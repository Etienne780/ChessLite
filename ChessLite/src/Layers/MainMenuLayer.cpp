#include "Layers/MainMenuLayer.h"
#include "App.h"

#include <CoreChessLib/CoreChess.h>

namespace Layers {

	void MainMenuLayer::OnStart(AppContext* ctx) {
		using namespace CoreChess;
		using namespace Internal;

		// setup Chess piece
		auto& reg = ChessPieceRegistry::GetInstance();
		
		ChessPieceID pawnID;
		auto* pawnPtr = reg.AddChessPiece(pawnID, "pawn");
		pawnPtr->SetMoveProperties(1, false, TargetType::FREE);
		pawnPtr->AddMoveRule(0, 1);

		pawnPtr->SetTargetType(TargetType::OPPONENT);
		pawnPtr->AddMoveRule(1, 1);
		pawnPtr->AddMoveRule(-1, 1);

		ChessContext chessCTX;
		chessCTX.AddPiece(pawnID);

		ChessGame p{ chessCTX };
	}

	void MainMenuLayer::OnUpdate(AppContext* ctx) {
		
	}

	void MainMenuLayer::OnRender(AppContext* ctx) {

	}

	void MainMenuLayer::OnQuit(AppContext* ctx) {
		
	}

}