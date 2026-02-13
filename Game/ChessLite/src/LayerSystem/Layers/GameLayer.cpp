#include "LayerSystem/Layers/GameLayer.h"
#include "LayerSystem/Layers/EscapeMenuLayer.h"
#include "Styles/Comman/Color.h"
#include "App.h"

namespace Layers {

	void GameLayer::OnStart(AppContext* ctx) {
		m_escapeMenuCloseEventID = ctx->app->SubscribeToLayerEvent<LayerEventType::CLOSED>(
		[&](const LayerEvent& e) -> void {
			if (e.layerID == LayerID::ESCAPE_MENU) {
				m_isEscapeMenuOpen = false;
			}
		});

		auto winID = ctx->app->GetWinID();
		auto* win = ctx->app->GetWindow(winID);
		if (win) {
			m_windowSize = win->GetSize();
			m_windowResizeCBID = win->AddOnWindowResize([this](const SDLCore::Window& win) {
				m_windowSize = win.GetSize();
			});
		}
		else {
			Log::Error("GameLayer: Failed to get window!");
		}

		SetupGame();
	}

	void GameLayer::OnUpdate(AppContext* ctx) {
		using namespace SDLCore;
		
		if (!m_isEscapeMenuOpen && Input::KeyJustPressed(KeyCode::ESCAPE)) {
			m_isEscapeMenuOpen = true;
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

		auto winID = ctx->app->GetWinID();
		auto* win = ctx->app->GetWindow(winID);
		if (win) {
			win->RemoveOnWindowResize(m_windowResizeCBID);
		}
	}

	LayerID GameLayer::GetLayerID() const {
		return LayerID::GAME;
	}

	void GameLayer::SetupGame() {
		using namespace CoreChess;

		auto& reg = ChessPieceRegistry::GetInstance();

		// --- Pawn Setup ---
		ChessPieceID pawnID;
		auto* pawn = reg.AddChessPiece(pawnID, "pawn", 1);
		pawn->SetMoveProperties(1, false, false, TargetType::FREE);
		pawn->AddMoveRule(0, 1); // forward
		pawn->SetTargetType(TargetType::OPPONENT);
		pawn->AddMoveRule(1, 1); // capture
		pawn->AddMoveRule(-1, 1);

		// --- Chess Board Setup ---
		ChessContext chessCTX;
		chessCTX.SetBoardSize(3, 3);
		chessCTX.BoardCmdFillRow(0, pawnID);

		//--- Win Condition Setup
		chessCTX.SetWinCondition([pawnID](const ChessGame& game) -> ChessWinResult {
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

	void GameLayer::RenderBoard() {
		namespace RE = SDLCore::Render;
		typedef SDLCore::UI::UIRegistry UIReg;

		Vector4 colorBoardDark;
		Vector4 colorBoardLight;

		UIReg::TryGetRegisteredColor(Style::commanColorBoardDark, colorBoardDark);
		UIReg::TryGetRegisteredColor(Style::commanColorBoardLight, colorBoardLight);

		const auto& board = m_game.GetBoard();
		int boardWidth = board.GetWidth();
		int boardHeight = board.GetHeight();

		Vector2 topLeftBoard{
			(m_windowSize.x * 0.5f) - (static_cast<float>(boardWidth) * m_boardTileSize * 0.5f),
			(m_windowSize.y * 0.5f) - (static_cast<float>(boardHeight) * m_boardTileSize * 0.5f)
		};
		
		RE::SetColor(colorBoardDark);
		for (int i = 0; i < boardHeight * boardWidth; i++) {
			if (i % 2 == 0) {
				int localX = i % boardWidth;
				int localY = static_cast<int>(i / boardHeight);

				float x = topLeftBoard.x + (static_cast<float>(localX) * m_boardTileSize);
				float y = topLeftBoard.y + (static_cast<float>(localY) * m_boardTileSize);

				RE::FillRect(x, y, m_boardTileSize, m_boardTileSize);
			}
		}

		RE::SetColor(colorBoardLight);
		for (int i = 0; i < boardHeight * boardWidth; i++) {
			if (i % 2 == 1) {
				int localX = i % boardWidth;
				int localY = static_cast<int>(i / boardHeight);

				float x = topLeftBoard.x + (static_cast<float>(localX) * m_boardTileSize);
				float y = topLeftBoard.y + (static_cast<float>(localY) * m_boardTileSize);

				RE::FillRect(x, y, m_boardTileSize, m_boardTileSize);
			}
		}
	}

}