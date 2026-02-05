#include "ChessBoard.h"

namespace CoreChess {

	ChessBoard::ChessBoard(int w, int h) 
		: m_boardWidth(w), m_boardHeight(h) {
		m_board = GenerateBoard(w, h);
	}

	bool ChessBoard::HasPieceAt(const Vector2& pos) const {
		return HasPieceAt(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	bool ChessBoard::HasPieceAt(int x, int y) const {
		size_t index = PosToIndex(x, y);
		const ChessField& f = m_board.at(index);
		return !f.IsPieceNone();
	}

	bool ChessBoard::IsPieceAtEqual(const Vector2& pos, FieldType type) const {
		return IsPieceAtEqual(static_cast<int>(pos.x), static_cast<int>(pos.y), type);
	}

	bool ChessBoard::IsPieceAtEqual(int x, int y, FieldType type) const {
		size_t index = PosToIndex(x, y);
		const ChessField& f = m_board.at(index);
		return f.GetFieldType() == type;
	}

	ChessField ChessBoard::GetFieldAt(const Vector2& pos) const {
		return GetFieldAt(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	ChessField ChessBoard::GetFieldAt(int x, int y) const {
		size_t index = PosToIndex(x, y);
		return m_board.at(index);
	}

	std::vector<ChessField> ChessBoard::GenerateBoard(int w, int h) {
		size_t count = static_cast<size_t>(w * h);
		std::vector<ChessField> result;
		result.reserve(count);
		for (size_t i = 0; i < count; i++) {
			result.emplace_back(ChessPieceID(CORE_CHESS_INVALID_ID), FieldType::NONE);
		}
		return result;
	}

	size_t ChessBoard::PosToIndex(int x, int y) const {
		return static_cast<size_t>(x + (m_boardWidth * y));
	}

}