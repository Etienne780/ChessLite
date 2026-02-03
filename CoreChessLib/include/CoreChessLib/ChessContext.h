#pragma once
#include <vector>

#include "ChessTypes.h"
#include "Internal/ChessBoard.h"

namespace CoreChess {

	/*
	* @stores data for a chess game like what pieces there are.
	* 
	* what board, what the starting position...
	*/
	class ChessContext {
	public:
		

	private:
		std::vector<ChessPieceID> m_pieces;
	};

}