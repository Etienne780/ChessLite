#include "ChessField.h"

namespace CoreChess {

	ChessField::ChessField(ChessPieceID piece, FieldType type)
		: m_piece(piece), m_type(type) {
	}

	bool ChessField::IsPieceNone() const {
		return m_type == FieldType::NONE;
	}

	bool ChessField::IsPieceWhite() const {
		return m_type == FieldType::WHITE;
	}

	bool ChessField::IsPieceBlack() const {
		return m_type == FieldType::BLACK;
	}

	ChessPieceID ChessField::GetPieceID() const {
		return m_piece;
	}

	FieldType ChessField::GetFieldType() const {
		return m_type;
	}

}