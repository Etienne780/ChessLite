#include "ChessContext.h"

namespace CoreChess {

	ChessContext& ChessContext::ReservePiece(size_t amount) {
		m_pieces.reserve(amount);
		return *this;
	}

	ChessContext& ChessContext::AddPiece(ChessPieceID pieceID) {
		m_pieces.push_back(pieceID);
		return *this;
	}

	ChessContext& ChessContext::SetBoardSize(int width, int height) {
		m_boardWidth = width;
		m_boardHeight = height;
		return *this;
	}

}