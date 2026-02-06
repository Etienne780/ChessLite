#include "Layers/MainMenuLayer.h"
#include "App.h"

#include <CoreChessLib/CoreChess.h>

namespace Layers {

	static CoreChess::ChessGame game;
	static Vector2 pos = Vector2::zero;

	void MainMenuLayer::OnStart(AppContext* ctx) {
		using namespace CoreChess;
		using namespace Internal;

		auto& reg = ChessPieceRegistry::GetInstance();

		// --- Pawn Setup ---
		ChessPieceID pawnID;
		auto* pawn = reg.AddChessPiece(pawnID, "pawn", 1);
		pawn->SetMoveProperties(1, false, TargetType::FREE);
		pawn->AddMoveRule(0, 1); // forward
		pawn->SetTargetType(TargetType::OPPONENT);
		pawn->AddMoveRule(1, 1); // capture
		pawn->AddMoveRule(-1, 1);

		// --- Chess Board Setup ---
		ChessContext chessCTX;
		chessCTX.SetBoardSize(3, 3);
		chessCTX.BoardCmdFillRow(0, pawnID);

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
	}

	void MainMenuLayer::OnRender(AppContext* ctx) {
		namespace RE = SDLCore::Render;

		RE::SetColor(255);
		RE::SetTextSize(36);
		RE::SetTextAlign(SDLCore::Align::CENTER);
		RE::Text(strBoard, 400, 400);
	}

	void MainMenuLayer::OnQuit(AppContext* ctx) {
	}

}