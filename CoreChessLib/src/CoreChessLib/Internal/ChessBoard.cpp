#include "Internal/ChessBoard.h"

namespace CoreChess::Internal {

	Field::Field(ChessPieceID piece, FieldType type)
		: m_piece(piece), m_type(type) {
	}

	ChessPieceID Field::GetPieceID() const {
		return m_piece;
	}

	FieldType Field::GetFieldType() const {
		return m_type;
	}

	bool Field::IsPieceNone() const {
		return m_type == FieldType::NONE;
	}

	bool Field::IsPieceWhite() const {
		return m_type == FieldType::WHITE;
	}

	bool Field::IsPieceBlack() const {
		return m_type == FieldType::BLACK;
	}

	ChessBoard::ChessBoard(int x, int y) 
		: m_boardSizeX(x), m_boardSizeY(y) {
	}

}