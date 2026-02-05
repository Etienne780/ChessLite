#pragma once
#include <vector>

#include "ChessTypes.h"
#include "ChessBoard.h"

namespace CoreChess {

	/*
	* @stores data for a chess game like what pieces there are.
	* 
	* what board, what the starting position...
	*/
	class ChessContext {
	public:
		ChessContext() = default;

		ChessContext& ReservePiece(size_t amount);
		ChessContext& AddPiece(ChessPieceID pieceID);

		ChessContext& SetBoardSize(int width, int height);

	private:
		int m_boardWidth = 0;
		int m_boardHeight = 0;
		std::vector<ChessPieceID> m_pieces;
	};

}