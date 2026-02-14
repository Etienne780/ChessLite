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

			Vector4 bounds = win->GetDisplayBounds();
			m_displaySize.Set(bounds.z, bounds.w);
			m_windowDisplayChangedCBID = win->AddOnWindowDisplayChanged([this](const SDLCore::Window& win) {
				Vector4 bounds = win.GetDisplayBounds();
				m_displaySize.Set(bounds.z, bounds.w);
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
			win->RemoveOnWindowDisplayChanged(m_windowDisplayChangedCBID);
		}
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

		float scaleX = m_displaySize.x / m_RefDisplaySize.x;
		float scaleY = m_displaySize.y / m_RefDisplaySize.y;
		float displayScale = std::min(scaleX, scaleY);

		float boardTileSize = m_boardTileSize * displayScale;

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
		}

		Vector2 topLeftBoard{
			(m_windowSize.x * 0.5f) - (static_cast<float>(boardWidth) * boardTileSize * 0.5f),
			(m_windowSize.y * 0.5f) - (static_cast<float>(boardHeight) * boardTileSize * 0.5f)
		};
		
		// render board

		RE::SetColor(colorBoardDark);
		for (int i = 0; i < boardWidth * boardHeight; i++) {
			int localX = i % boardWidth;
			int localY = i / boardWidth;

			if ((localX + localY) % 2 == 0) {
				float x = topLeftBoard.x + static_cast<float>(localX) * boardTileSize;
				float y = topLeftBoard.y + static_cast<float>(localY) * boardTileSize;

				RE::FillRect(x, y, boardTileSize, boardTileSize);
			}
		}

		RE::SetColor(colorBoardLight);
		for (int i = 0; i < boardHeight * boardWidth; i++) {
			int localX = i % boardWidth;
			int localY = i / boardWidth;

			if ((localX + localY) % 2 == 1) {
				float x = topLeftBoard.x + (static_cast<float>(localX) * boardTileSize);
				float y = topLeftBoard.y + (static_cast<float>(localY) * boardTileSize);

				RE::FillRect(x, y, boardTileSize, boardTileSize);
			}
		}

		// render board
	}

}