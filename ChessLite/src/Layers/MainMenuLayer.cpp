#include "Layers/MainMenuLayer.h"
#include "App.h"

#include <CoreChessLib/CoreChess.h>

namespace Layers {

	static CoreChess::ChessGame game;
	static Vector2 pos = Vector2::zero;
	static bool gameEnded = false;
	static CoreChess::ChessWinResult gameResult = CoreChess::ChessWinResult::NONE;


	void MainMenuLayer::OnStart(AppContext* ctx) {
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

		game.SetGameContext(chessCTX);
		game.StartGame();
	}


	static void AppendField(const CoreChess::ChessField& field, std::string& str) {
		auto type = field.GetFieldType();
		switch (type) {
		case CoreChess::FieldType::NONE:
			str += "0";
			break;
		case CoreChess::FieldType::WHITE:
			str += "2";
			break;
		case CoreChess::FieldType::BLACK:
			str += "3";
			break;
		default:
			break;
		}
	}

	static void MoveCursor(int w, int h) {
		using namespace SDLCore;

		if (Input::KeyJustPressed(KeyCode::W) || Input::KeyJustPressed(KeyCode::UP)) {
			pos.y -= 1;
		}
		if (Input::KeyJustPressed(KeyCode::S) || Input::KeyJustPressed(KeyCode::DOWN)) {
			pos.y += 1;
		}
		if (Input::KeyJustPressed(KeyCode::A) || Input::KeyJustPressed(KeyCode::LEFT)) {
			pos.x -= 1;
		}
		if (Input::KeyJustPressed(KeyCode::D) || Input::KeyJustPressed(KeyCode::RIGHT)) {
			pos.x += 1;
		}

		pos.x = static_cast<float>(std::clamp(static_cast<int>(pos.x), 0, w - 1));
		pos.y = static_cast<float>(std::clamp(static_cast<int>(pos.y), 0, h - 1));
	}
	
	static std::string strBoard;
	void MainMenuLayer::OnUpdate(AppContext* ctx) {
		strBoard.clear();
		const auto& board = game.GetBoard();
		int boardW = board.GetWidth();
		int boardH = board.GetHeight();

		MoveCursor(boardW, boardH);

		if (SDLCore::Input::KeyJustPressed(SDLCore::KeyCode::SPACE)) {
			if (game.IsPieceSelected()) {
				if (pos == game.GetSelectedPiecePos()) {
					game.DeselectPiece();
				}
				else if(game.MovePiece(pos)){
				}
				else {
					game.SelectPiece(pos);
				}
			}
			else {
				game.SelectPiece(pos);
			}
		}

		const auto& fields = board.GetFields();
		std::vector<size_t> possibleMoveIndices;

		if (game.IsPieceSelected()) {
			size_t index = game.GetSelectedPieceIndex();
			possibleMoveIndices = game.GetPossibleMoveIndicesOf(index);
		}

		for (size_t i = 0; i < fields.size(); i++) {
			const auto& f = fields[i];

			size_t index = game.ConverToBoardIndex(pos);
			if (i == index) {
				strBoard += "X";
			}
			else if (auto it = std::find(possibleMoveIndices.begin(), possibleMoveIndices.end(), i); 
				it != possibleMoveIndices.end()) {
				strBoard += "#";
			}
			else {
				AppendField(f, strBoard);
			}

			if (((static_cast<int>(i) + 1) % boardW) == 0) {
				strBoard += "\n";
			}
		}

		CoreChess::ChessWinResult result;
		if (!gameEnded) {
			CoreChess::ChessWinResult result;
			if (game.IsGameEnd(result)) {
				gameEnded = true;
				gameResult = result;
			}
		}
	}

	void MainMenuLayer::OnRender(AppContext* ctx) {
		namespace RE = SDLCore::Render;

		RE::SetColor(255);
		RE::SetTextSize(36);
		RE::SetTextAlign(SDLCore::Align::CENTER);
		RE::Text(strBoard, 400, 400);

		// --- Material display ---
		RE::SetTextSize(24);
		RE::SetTextAlign(SDLCore::Align::START);

		int blackPoints = game.GetBlackMaterialValue();
		int whitePoints = game.GetWhiteMaterialValue();

		RE::TextF(20, 20, "Black: {}", blackPoints);
		RE::TextF(20, 50, "White: {}", whitePoints);

		// --- Game result display ---
		if (gameEnded) {
			RE::SetTextSize(40);
			RE::SetTextAlign(SDLCore::Align::CENTER);

			switch (gameResult) {
			case CoreChess::ChessWinResult::WHITE_WON:
				RE::Text("WHITE WINS", 400, 100);
				break;

			case CoreChess::ChessWinResult::BLACK_WON:
				RE::Text("BLACK WINS", 400, 100);
				break;

			case CoreChess::ChessWinResult::DRAW:
				RE::Text("DRAW", 400, 100);
				break;

			default:
				break;
			}
		}
	}

	void MainMenuLayer::OnQuit(AppContext* ctx) {
	}

}