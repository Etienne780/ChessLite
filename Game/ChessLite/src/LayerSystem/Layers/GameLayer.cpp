#include <CoreLib/Random.h>

#include "LayerSystem/Layers/GameLayer.h"
#include "LayerSystem/Layers/GameResult.h"
#include "LayerSystem/Layers/EscapeMenuLayer.h"
#include "LayerSystem/Layers/AIVisualizerLayer.h"
#include "Styles/Comman/Color.h"
#include "Styles/Comman/Space.h"
#include "Styles/Comman/Style.h"
#include "App.h"

namespace UI = SDLCore::UI;

namespace Layers {

	GameLayer::GameLayer(PlayerType player1, PlayerType player2) 
		: m_player1(player1), m_player2(player2) {
	}

	void GameLayer::OnStart(AppContext* ctx) {
		namespace Prop = UI::Properties;

		m_root
			.Merge(Style::commanRootTransparent)
			.SetValue(Prop::positionType, SDLCore::UI::UIPositionType::ABSOLUTE)
			.SetValue(Prop::padding, Style::commanSpaceL);

		m_pawnLightTexture = std::make_shared<SDLCore::Texture>(SDLCore::TEXTURE_FALLBACK_TEXTURE);
		m_pawnDarkTexture = std::make_shared<SDLCore::Texture>(SDLCore::TEXTURE_FALLBACK_TEXTURE);

		m_menuCloseEventID = ctx->app->SubscribeToLayerEvent<LayerEventType::CLOSED>(
		[&](const LayerEvent& e) -> void {
			if (e.layerID == LayerID::ESCAPE_MENU) {
				m_isEscapeMenuOpen = false;
			}

			if (e.layerID == LayerID::AI_VISUALIZER) {
				m_isAIVisualizerOpen = false;
			}

			if (e.layerID == LayerID::GAME_RESULT) {
				m_opendGameResult = false;
				StartGame();
			}
		});

		SetupGame();
		// setup Agents for testing
		ctx->agentManager.AddAgent(AgentID(0), Agent("name1", m_game.GetContext()));
		ctx->agentManager.AddAgent(AgentID(1), Agent("name2", m_game.GetContext()));
		ctx->selectedAgentID1 = AgentID(0);
		ctx->selectedAgentID2 = AgentID(1);

		m_agentID1 = ctx->selectedAgentID1;
		m_agentID2 = ctx->selectedAgentID2;

		auto ensureValidAIPlayer = [](AppContext* ctx, PlayerType& type, AgentID agentID) -> void {
			if (type == PlayerType::AI && agentID.IsInvalid()) {
				type = PlayerType::PLAYER;
				return;
			}

			auto* agent = ctx->agentManager.GetAgent(agentID);
			if (!agent) {
				type = PlayerType::PLAYER;
			}
		};

		ensureValidAIPlayer(ctx, m_player1, m_agentID1);
		ensureValidAIPlayer(ctx, m_player2, m_agentID2);

		StartGame();
	}

	void GameLayer::OnUpdate(AppContext* ctx) {
		using namespace SDLCore;
		
		m_options = ctx->options;
		m_RefDisplaySize = ctx->refDisplaySize;
		m_displaySize = ctx->displaySize;
		m_windowSize = ctx->windowSize;

		// update skin
		if (m_skinType != ctx->skinManager.GetCurrentSkin()) {
			m_skinType = ctx->skinManager.GetCurrentSkin();
			m_pawnLightTexture = ctx->skinManager.GetSkinLight();
			m_pawnDarkTexture = ctx->skinManager.GetSkinDark();
		}

		if (!m_opendGameResult && m_gameResult != ChessCoreResult::NONE) {
			m_opendGameResult = true;
			m_game.EndGame();
			ResetChessSelectedParams();
			ctx->app->PushLayer<GameResult>(m_gameResult == ChessCoreResult::WHITE_WON);
		}

		UpdateBoardTileSize();
		GameLogic();

		if (!m_isAIVisualizerOpen && 
			Input::KeyJustPressed(KeyCode::NUM_1)) {
			m_isAIVisualizerOpen = true;
			ctx->app->PushLayer<AIVisualizerLayer>(m_player1, m_player2);
		}

		if (!m_opendGameResult && 
			!m_isEscapeMenuOpen && 
			Input::KeyJustPressed(KeyCode::ESCAPE)) 
		{
			m_isEscapeMenuOpen = true;
			ResetChessSelectedParams();
			ctx->app->PushLayer<EscapeMenuLayer>();
		}
	}

	void GameLayer::OnRender(AppContext* ctx) {
		RenderBoard();
	}

	void GameLayer::OnUIRender(AppContext* ctx) {
		typedef UI::UIKey Key;

		std::string playerTypeStr = "Turn: ";
		playerTypeStr += (m_isPlayer1Turn) ? 
			FormatUtils::formatString("Player1 ({})", m_player1) :
			FormatUtils::formatString("Player2 ({})", m_player2);

		UI::BeginFrame(Key("game_ui_overlay"), m_root);
		{
			UI::Text(Key("title"), playerTypeStr, Style::commanTextBase);
		}
		UI::EndFrame();
	}

	void GameLayer::OnQuit(AppContext* ctx) {
		m_game.EndGame();
		ctx->app->UnsubscribeToLayerEvent(LayerEventType::CLOSED, m_menuCloseEventID);
	}

	LayerID GameLayer::GetLayerID() const {
		return LayerID::GAME;
	}

	void GameLayer::SetupGame() {
		using namespace CoreChess;

		auto& reg = ChessPieceRegistry::GetInstance();

		// --- Pawn Setup ---
		auto* pawn = reg.AddChessPiece(m_pawnID, "pawn", 1);
		pawn->SetMoveProperties(1, false, false, TargetType::FREE);
		pawn->AddMoveRule(0, 1); // forward
		pawn->SetTargetType(TargetType::OPPONENT);
		pawn->AddMoveRule(1, 1); // capture
		pawn->AddMoveRule(-1, 1);

		// --- Chess Board Setup ---
		ChessContext chessCTX;
		chessCTX.SetBoardSize(3, 3);
		chessCTX.BoardCmdFillRow(0, m_pawnID);

		//--- Win Condition Setup
		chessCTX.SetWinCondition([this](const ChessGame& game) -> ChessWinResult {
			if (game.IsWhiteTurn()) {
				if (!game.HasAnyLegalMove(FieldType::BLACK)) {
					return ChessWinResult::WHITE_WON;
				}
			}
			else {
				if (!game.HasAnyLegalMove(FieldType::WHITE)) {
					return ChessWinResult::BLACK_WON;
				}
			}

			const auto& board = game.GetBoard();
			int w = board.GetWidth();
			int h = board.GetHeight();

			// White reaches top row
			for (size_t x = 0; x < w; ++x) {
				ChessField f = board.GetFieldAt(static_cast<int>(x), 0);
				if (f.GetFieldType() == FieldType::WHITE)
					return ChessWinResult::WHITE_WON;
			}

			// Black reaches bottom row
			for (size_t x = 0; x < w; ++x) {
				ChessField f = board.GetFieldAt(static_cast<int>(x), h - 1);
				if (f.GetFieldType() == FieldType::BLACK)
					return ChessWinResult::BLACK_WON;
			}

			return ChessWinResult::NONE;
		});

		m_game.SetGameContext(std::move(chessCTX));
	}

	void GameLayer::StartGame() {
		m_gameResult = ChessCoreResult::NONE;
		m_gameEnded = false;

		int random = Random::GetRangeNumber<int>(0, 1);
		m_isPlayer1Turn = /*(random == 1)*/false;
		m_player1White = m_isPlayer1Turn;

		m_game.StartGame();
	}

	void GameLayer::GameLogic() {
		if (!m_gameEnded && m_game.IsGameEnd(&m_gameResult)) {
			m_gameEnded = true;
			EvaluateAIs();
			ResetChessSelectedParams();
			return;
		}

		if (m_gameEnded || m_isEscapeMenuOpen)
			return;

		if (m_isPlayer1Turn) {
			ProcessTurn(m_player1);
		}
		else {
			ProcessTurn(m_player2);
		}
	}

	void GameLayer::EvaluateAIs() {
		auto* app = App::GetInstance();
		if (!app) {
			throw std::runtime_error("EvaluateAIs: app was nullptr!");
		}
		auto* ctx = app->GetContext();
		Agent* agent1 = ctx->agentManager.GetAgent(m_agentID1);
		Agent* agent2 = ctx->agentManager.GetAgent(m_agentID2);
		if (!agent1 || !agent2) {
			throw std::runtime_error("EvaluateAIs: agent with id doesnt exist!");
		}
		
		ChessCoreResult result = ChessCoreResult::NONE;
		if (m_game.IsGameEnd(&result)) {
			agent1->GameFinished(
				(m_player1White && result == ChessCoreResult::WHITE_WON) ||
				(!m_player1White && result == ChessCoreResult::BLACK_WON)
			);

			agent2->GameFinished(
				(!m_player1White && result == ChessCoreResult::WHITE_WON) ||
				(m_player1White && result == ChessCoreResult::BLACK_WON)
			);
		}
	}

	void GameLayer::ProcessTurn(PlayerType type) {
		bool movePlayed = false;
		


		switch (type) {
		case PlayerType::PLAYER:
			movePlayed = PlayerLogic();
			break;
		case PlayerType::AI:
			movePlayed = AILogic();
			break;
		default:
			break;
		}

		if(movePlayed) 
			m_isPlayer1Turn = !m_isPlayer1Turn;
	}

	bool GameLayer::PlayerLogic() {
		using namespace SDLCore;

		const auto& board = m_game.GetBoard();
		int boardWidth = board.GetWidth();
		int boardHeight = board.GetHeight();

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float boardTileSize = m_boardTileSizeFinal;
		float tileScaler = m_tileScaler;

		Vector2 topLeftBoard{
			(m_windowSize.x * 0.5f) - (static_cast<float>(boardWidth) * boardTileSize * 0.5f),
			(m_windowSize.y * 0.5f) - (static_cast<float>(boardHeight) * boardTileSize * 0.5f)
		};

		bool isWhiteTurn = m_game.IsWhiteTurn();

		Vector2 mPos = Input::GetMousePosition();
		bool mLeftJustClick = Input::MouseJustPressed(MouseButton::LEFT);
		bool mLeftClick = Input::MousePressed(MouseButton::LEFT);

		bool movePlayed = false;

		for (int i = 0; i < boardWidth * boardHeight; i++) {
			int localX = i % boardWidth;
			int localY = i / boardWidth;

			float x = topLeftBoard.x + static_cast<float>(localX) * boardTileSize;
			float y = topLeftBoard.y + static_cast<float>(localY) * boardTileSize;

			auto field = board.GetFieldAt(i);
			bool canPlay = (isWhiteTurn) ?
				field.GetFieldType() == CoreChess::FieldType::WHITE :
				field.GetFieldType() == CoreChess::FieldType::BLACK;

			if (!IsPointInRect(mPos, x, y, boardTileSize))
				continue;

			if (!mLeftJustClick)
				break;

			if (!field.IsPieceNone() &&
				canPlay) {
				if (m_pieceSelected &&
					m_selectedPieceLocalPos.Equals(static_cast<float>(localX), static_cast<float>(localY))) {
					ResetChessSelectedParams();
					break;
				}
				m_pieceSelected = true;
				m_selectedPieceID = field.GetPieceID();
				m_selectedPieceLocalPos.Set(static_cast<float>(localX), static_cast<float>(localY));
				m_selectedPiecePos.Set(x, y);
			}
			else {
				if (m_pieceSelected) {
					movePlayed = TryMovePiece(
						m_selectedPieceLocalPos,
						static_cast<float>(localX),
						static_cast<float>(localY)
					);
				}
				ResetChessSelectedParams();
			}

			break;
		}

		return movePlayed;
	}

	bool GameLayer::AILogic() {
		AgentID agentID = (m_isPlayer1Turn) ? m_agentID1 : m_agentID2;
		bool agentIsWhite = m_game.IsWhiteTurn();

		auto* app = App::GetInstance();
		if (!app) {
			throw std::runtime_error("AILogic: app was nullptr!");
		}
		auto* ctx = app->GetContext();
		Agent* agent = ctx->agentManager.GetAgent(agentID);
		if (!agent) {
			throw std::runtime_error("AILogic: agent with id doesnt exist!");
		}

		bool movePlayed = false;
		do {
			const GameMove& move = agent->GetBestMove(m_game);

			movePlayed = TryMovePiece(
				move.GetFrom(), 
				move.GetTo()
			);
			Log::Print("AI: {} Move, from {} -> to {}", ((agentIsWhite) ? "White" : "Black"), move.GetFrom(), move.GetTo());
		} while (!movePlayed);

		return movePlayed;
	}

	void GameLayer::RenderBoard() {
		namespace RE = SDLCore::Render;
		typedef SDLCore::UI::UIRegistry UIReg;

		Vector4 colorBtnNormal;
		Vector4 colorOverlay;
		Vector4 colorHighlightSelected;
		Vector4 colorHighlightMove;
		Vector4 colorBoardDark;
		Vector4 colorBoardLight;
		
		UIReg::TryGetRegisteredColor(Style::commanColorBtnNormal, colorBtnNormal);
		UIReg::TryGetRegisteredColor(Style::commanColorOverlay, colorOverlay);
		UIReg::TryGetRegisteredColor(Style::commanColorHighlightSelected, colorHighlightSelected);
		UIReg::TryGetRegisteredColor(Style::commanColorHighlightMove, colorHighlightMove);
		UIReg::TryGetRegisteredColor(Style::commanColorBoardDark, colorBoardDark);
		UIReg::TryGetRegisteredColor(Style::commanColorBoardLight, colorBoardLight);

		const auto& board = m_game.GetBoard();
		int boardWidth = board.GetWidth();
		int boardHeight = board.GetHeight();

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float boardTileSize = m_boardTileSizeFinal;
		float tileScaler = m_tileScaler;

		Vector2 topLeftBoard{
			(m_windowSize.x * 0.5f) - (static_cast<float>(boardWidth) * boardTileSize * 0.5f),
			(m_windowSize.y * 0.5f) - (static_cast<float>(boardHeight) * boardTileSize * 0.5f)
		};

		auto DrawCheckPattern = [&](int tileType, const Vector4& color) {
			RE::SetColor(color);

			for (int i = 0; i < boardWidth * boardHeight; i++) {
				int localX = i % boardWidth;
				int localY = i / boardWidth;

				if ((localX + localY) % 2 != tileType)
					continue;

				float x = topLeftBoard.x + static_cast<float>(localX) * boardTileSize;
				float y = topLeftBoard.y + static_cast<float>(localY) * boardTileSize;

				RE::FillRect(x, y, boardTileSize, boardTileSize);
			}
		};

		// render board
		DrawCheckPattern(0, colorBoardLight);
		DrawCheckPattern(1, colorBoardDark);

		// render possible moves
		if (m_pieceSelected && m_options.showPossibleMoves) {
			auto& reg = CoreChess::ChessPieceRegistry::GetInstance();
			const CoreChess::ChessPiece* piece = reg.GetChessPiece(m_selectedPieceID);

			if (piece) {
				RE::SetColor(colorHighlightMove);
				for (int i = 0; i < boardWidth * boardHeight; i++) {
					int localX = i % boardWidth;
					int localY = i / boardWidth;

					float x = topLeftBoard.x + static_cast<float>(localX) * boardTileSize;
					float y = topLeftBoard.y + static_cast<float>(localY) * boardTileSize;

					if (piece->IsValidMove(board, m_selectedPieceLocalPos,
						Vector2{ static_cast<float>(localX), static_cast<float>(localY) })) {
						RE::FillRect(x, y, boardTileSize, boardTileSize);
					}
				}
			}
		}

		// render board outline
		RE::SetColor(colorBtnNormal);
		RE::SetInnerStroke(false);
		RE::SetStrokeWidth(8.0f * tileScaler);
		RE::Rect(topLeftBoard, Vector2(static_cast<float>(boardWidth), static_cast<float>(boardHeight)) * boardTileSize);

		// render Figures
		RE::SetColor(colorHighlightSelected);
		RE::SetInnerStroke(true);
		RE::SetStrokeWidth(4.0f * tileScaler);
		for (int i = 0; i < boardHeight * boardWidth; i++) {
			auto field = board.GetFieldAt(i);

			if (field.GetPieceID() != m_pawnID)
				continue;

			int localX = i % boardWidth;
			int localY = i / boardWidth;

			float x = topLeftBoard.x + static_cast<float>(localX) * boardTileSize;
			float y = topLeftBoard.y + static_cast<float>(localY) * boardTileSize;

			bool selected = m_selectedPiecePos.Equals(x, y) && m_pieceSelected;

			if (selected)
				RE::Rect(x, y, boardTileSize, boardTileSize);

			if (field.GetFieldType() == CoreChess::FieldType::BLACK) {
				if (!m_pawnDarkTexture)
					continue;

				RE::Texture(*m_pawnDarkTexture, x, y, boardTileSize, boardTileSize);
			}
			else if (field.GetFieldType() == CoreChess::FieldType::WHITE) {
				if (!m_pawnLightTexture)
					continue;

				RE::Texture(*m_pawnLightTexture, x, y, boardTileSize, boardTileSize);
			}
		}
	}

	void GameLayer::UpdateBoardTileSize() {
		int boardWidth = m_game.GetBoard().GetWidth();
		int boardHeight = m_game.GetBoard().GetHeight();

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float baseTileSize = m_boardTileSize * displayScale;

		if (m_calculateBoardTileSize) {
			float top = m_boardMargin.x * displayScale;
			float left = m_boardMargin.y * displayScale;
			float bottom = m_boardMargin.z * displayScale;
			float right = m_boardMargin.w * displayScale;

			float usableWidth = m_windowSize.x - left - right;
			float usableHeight = m_windowSize.y - top - bottom;

			float boardTileSizeX = usableWidth / boardWidth;
			float boardTileSizeY = usableHeight / boardHeight;

			m_boardTileSizeFinal = std::min(boardTileSizeX, boardTileSizeY);
			m_tileScaler = m_boardTileSizeFinal / baseTileSize;
		}
		else {
			m_boardTileSizeFinal = baseTileSize;
			m_tileScaler = 1.0f;
		}
	}

	void GameLayer::ResetChessSelectedParams() {
		m_pieceSelected = false;
		m_selectedPieceID.SetInvalid();
		m_selectedPieceLocalPos.Set(0);
		m_selectedPiecePos.Set(0);
	}

	bool GameLayer::TryMovePiece(const Vector2& from, const Vector2& to) {
		return TryMovePiece(from, to.x, to.y);
	}

	bool GameLayer::TryMovePiece(const Vector2& from, float toX, float toY) {
		m_game.SelectPiece(from);
		return m_game.MovePiece(toX, toY);
	}

	bool GameLayer::IsPointInRect(Vector2 mPos, float x, float y, float size) {
		return (mPos.x > x && mPos.x < x + size) &&
			(mPos.y > y && mPos.y < y + size);
	}
}