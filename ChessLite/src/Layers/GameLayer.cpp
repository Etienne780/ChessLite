#include "Layers/GameLayer.h"
#include "App.h"

namespace Layers {

	void GameLayer::OnStart(AppContext* ctx) {
		SetupGame();
	}

	void GameLayer::OnUpdate(AppContext* ctx) {

	}

	void GameLayer::OnRender(AppContext* ctx) {
		RenderBoard();
		RenderUI();
	}

	void GameLayer::OnQuit(AppContext* ctx) {
		m_game.EndGame();
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

	}

	void GameLayer::RenderUI() {

	}

}