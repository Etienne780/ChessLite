#include "LayerSystem/Layers/GameLayer.h"
#include "LayerSystem/Layers/GameResult.h"
#include "LayerSystem/Layers/EscapeMenuLayer.h"
#include "Styles/Comman/Color.h"
#include "App.h"

namespace Layers {

	void GameLayer::OnStart(AppContext* ctx) {
		m_pawnLightTexture = std::make_shared<SDLCore::Texture>(SDLCore::TEXTURE_FALLBACK_TEXTURE);
		m_pawnDarkTexture = std::make_shared<SDLCore::Texture>(SDLCore::TEXTURE_FALLBACK_TEXTURE);

		m_escapeMenuCloseEventID = ctx->app->SubscribeToLayerEvent<LayerEventType::CLOSED>(
		[&](const LayerEvent& e) -> void {
			if (e.layerID == LayerID::ESCAPE_MENU) {
				m_isEscapeMenuOpen = false;
			}
		});

		SetupGame();
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
			ctx->app->PushLayer<GameResult>(m_gameResult == ChessCoreResult::WHITE_WON);
		}

		GameLogic();

		if (!m_isEscapeMenuOpen && Input::KeyJustPressed(KeyCode::ESCAPE)) {
			m_isEscapeMenuOpen = true;
			ResetChessSelectedParams();
			ctx->app->PushLayer<EscapeMenuLayer>();
		}
	}

	void GameLayer::OnRender(AppContext* ctx) {
		RenderBoard();
	}

	void GameLayer::OnUIRender(AppContext* ctx) {
		
	}

	void GameLayer::OnQuit(AppContext* ctx) {
		m_game.EndGame();
		ctx->app->UnsubscribeToLayerEvent(LayerEventType::CLOSED, m_escapeMenuCloseEventID);
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

		m_game.SetGameContext(chessCTX);
		m_game.StartGame();
	}

	void GameLayer::GameLogic() {
		using namespace SDLCore;

		if (m_game.IsGameEnd(&m_gameResult)) {
			ResetChessSelectedParams();
			return;
		}

		const auto& board = m_game.GetBoard();
		int boardWidth = board.GetWidth();
		int boardHeight = board.GetHeight();

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float boardTileSize = m_boardTileSize * displayScale;

		float tileScaler = 1.0f;

		if (m_calculateBoardTileSize) {
			float top = m_boardMargin.x * displayScale;
			float left = m_boardMargin.y * displayScale;
			float bottom = m_boardMargin.z * displayScale;
			float right = m_boardMargin.w * displayScale;

			float usableWidth = m_windowSize.x - top - bottom;
			float usableHeight = m_windowSize.y - left - right;

			float boardTileSizeX = usableWidth / boardWidth;
			float boardTileSizeY = usableHeight / boardHeight;

			boardTileSize = std::min(boardTileSizeX, boardTileSizeY);

			tileScaler = boardTileSize / (m_boardTileSize * displayScale);
		}

		Vector2 topLeftBoard{
			(m_windowSize.x * 0.5f) - (static_cast<float>(boardWidth) * boardTileSize * 0.5f),
			(m_windowSize.y * 0.5f) - (static_cast<float>(boardHeight) * boardTileSize * 0.5f)
		};

		bool isWhiteTurn = m_game.IsWhiteTurn();

		Vector2 mPos = Input::GetMousePosition();
		bool mLeftJustClick = Input::MouseJustPressed(MouseButton::LEFT);
		bool mLeftClick = Input::MousePressed(MouseButton::LEFT);

		for (int i = 0; i < boardWidth * boardHeight; i++) {
			int localX = i % boardWidth;
			int localY = i / boardWidth;

			float x = topLeftBoard.x + static_cast<float>(localX) * boardTileSize;
			float y = topLeftBoard.y + static_cast<float>(localY) * boardTileSize;

			auto field = board.GetFieldAt(i);
			bool canPlay = (isWhiteTurn) ? 
				field.GetFieldType() == CoreChess::FieldType::WHITE : 
				field.GetFieldType() == CoreChess::FieldType::BLACK;

			if (IsPointInRect(mPos, x, y, boardTileSize)) {
				if (mLeftJustClick) {
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
						if(m_pieceSelected)
							TryMovePiece(m_selectedPieceLocalPos, static_cast<float>(localX), static_cast<float>(localY));
						ResetChessSelectedParams();
					}
				}

				break;
			}
		}
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

		float boardTileSize = m_boardTileSize * displayScale;

		float tileScaler = 1.0f;

		if (m_calculateBoardTileSize) {
			float top = m_boardMargin.x * displayScale;
			float left = m_boardMargin.y * displayScale;
			float bottom = m_boardMargin.z * displayScale;
			float right = m_boardMargin.w * displayScale;

			float usableWidth = m_windowSize.x - top - bottom;
			float usableHeight = m_windowSize.y - left - right;

			float boardTileSizeX = usableWidth / boardWidth;
			float boardTileSizeY = usableHeight / boardHeight;

			boardTileSize = std::min(boardTileSizeX, boardTileSizeY);

			tileScaler = boardTileSize / (m_boardTileSize * displayScale);
		}

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

	void GameLayer::ResetChessSelectedParams() {
		m_pieceSelected = false;
		m_selectedPieceID.SetInvalid();
		m_selectedPieceLocalPos.Set(0);
		m_selectedPiecePos.Set(0);
	}

	void GameLayer::TryMovePiece(const Vector2& from, float toX, float toY) {
		m_game.SelectPiece(from);
		m_game.MovePiece(toX, toY);
	}

	bool GameLayer::IsPointInRect(Vector2 mPos, float x, float y, float size) {
		return (mPos.x > x && mPos.x < x + size) &&
			(mPos.y > y && mPos.y < y + size);
	}
}