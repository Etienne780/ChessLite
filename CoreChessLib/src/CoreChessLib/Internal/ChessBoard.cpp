#include "Internal/ChessBoard.h"

namespace CoreChess::Internal {

	#pragma region Field

	Field::Field(ChessPieceID piece, FieldType type)
		: m_piece(piece), m_type(type) {
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

	ChessPieceID Field::GetPieceID() const {
		return m_piece;
	}

	FieldType Field::GetFieldType() const {
		return m_type;
	}

	#pragma endregion

	#pragma region ChessBoard

	ChessBoard::ChessBoard(int x, int y) 
		: m_boardSizeX(x), m_boardSizeY(y) {

		size_t count = static_cast<size_t>(x * y);
		m_board.reserve(count);
		for (size_t i = 0; i < count; i++) {
			m_board.emplace_back(ChessPieceID(CORE_CHESS_INVALID_ID), FieldType::NONE);
		}
	}

	bool ChessBoard::HasPieceAt(const Vector2& pos) const {
		return HasPieceAt(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	bool ChessBoard::HasPieceAt(int x, int y) const {
		size_t index = PosToIndex(x, y);
		const Field& f = m_board.at(index);
		return !f.IsPieceNone();
	}

	Field ChessBoard::GetFieldAt(const Vector2& pos) const {
		return GetFieldAt(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	Field ChessBoard::GetFieldAt(int x, int y) const {
		size_t index = PosToIndex(x, y);
		return m_board.at(index);
	}

	size_t ChessBoard::PosToIndex(int x, int y) const {
		return static_cast<size_t>(x + (m_boardSizeX * y));
	}

	#pragma endregion

}