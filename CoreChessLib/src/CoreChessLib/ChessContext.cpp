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

}