#include "ChessBoard.h"

namespace CoreChess {

	ChessBoard::ChessBoard(int w, int h) 
		: m_boardWidth(w), m_boardHeight(h) {
		m_board = GenerateBoard(w, h);
	}

	void ChessBoard::MovePieceFromTo(const Vector2& from, const Vector2& to) {
		return MovePieceFromTo(static_cast<int>(from.x), static_cast<int>(from.y),
			static_cast<int>(to.x), static_cast<int>(to.y));
	}

	void ChessBoard::MovePieceFromTo(int fromX, int fromY, int toX, int toY) {
		size_t fromIndex = PosToIndex(fromX, fromY);
		if (fromIndex >= m_board.size())
			return;

		ChessField& f = m_board.at(fromIndex);
		
		SetFieldAt(toX, toY, f.GetFieldType(), f.GetPieceID());
		SetFieldAt(fromX, fromY, FieldType::NONE);
	}

	void ChessBoard::MovePieceFromTo(size_t fromIndex, size_t toIndex) {
		if (fromIndex >= m_board.size())
			return;

		ChessField& f = m_board.at(fromIndex);

		SetFieldAt(toIndex, f.GetFieldType(), f.GetPieceID());
		SetFieldAt(fromIndex, FieldType::NONE);
	}

	void ChessBoard::SetFieldAt(const Vector2& at, FieldType type, ChessPieceID id) {
		return SetFieldAt(static_cast<int>(at.x), static_cast<int>(at.y), type, id);
	}

	void ChessBoard::SetFieldAt(int atX, int atY, FieldType type, ChessPieceID id) {
		size_t index = PosToIndex(atX, atY);
		return SetFieldAt(index, type, id);
	}

	void ChessBoard::SetFieldAt(size_t index, FieldType type, ChessPieceID id) {
		if (index >= m_board.size())
			return;

		ChessField& f = m_board.at(index);
		f = ChessField(id, type);
	}

	bool ChessBoard::HasPieceAt(const Vector2& pos) const {
		return HasPieceAt(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	bool ChessBoard::HasPieceAt(int x, int y) const {
		size_t index = PosToIndex(x, y);
		return HasPieceAt(index);
	}

	bool ChessBoard::HasPieceAt(size_t index) const {
		if (index >= m_board.size())
			return false;

		const ChessField& f = m_board.at(index);
		return !f.IsPieceNone();
	}

	size_t ChessBoard::PosToIndex(int x, int y) const {
		return static_cast<size_t>(x + (m_boardWidth * y));
	}

	Vector2 ChessBoard::IndexToPos(size_t index) const {
		int x = static_cast<int>(index % m_boardWidth);
		int y = static_cast<int>(index / m_boardWidth);
		return Vector2{ static_cast<float>(x), static_cast<float>(y) };
	}

	bool ChessBoard::IsPieceAtEqual(const Vector2& pos, FieldType type) const {
		return IsPieceAtEqual(static_cast<int>(pos.x), static_cast<int>(pos.y), type);
	}

	bool ChessBoard::IsPieceAtEqual(int x, int y, FieldType type) const {
		size_t index = PosToIndex(x, y);
		return IsPieceAtEqual(index, type);
	}

	bool ChessBoard::IsPieceAtEqual(size_t index, FieldType type) const {
		if (index >= m_board.size())
			return false;

		const ChessField& f = m_board.at(index);
		return f.GetFieldType() == type;
	}

	ChessField ChessBoard::GetFieldAt(const Vector2& pos) const {
		return GetFieldAt(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	ChessField ChessBoard::GetFieldAt(int x, int y) const {
		size_t index = PosToIndex(x, y);
		return GetFieldAt(index);
	}

	ChessField ChessBoard::GetFieldAt(size_t index) const {
		if (index >= m_board.size())
			return ChessField{};

		return m_board.at(index);
	}

	const std::vector<ChessField>& ChessBoard::GetFields() const {
		return m_board;
	}

	int ChessBoard::GetWidth() const {
		return m_boardWidth;
	}

	int ChessBoard::GetHeight() const {
		return m_boardHeight;
	}

	size_t ChessBoard::GetNumberOfFields() const {
		return static_cast<size_t>(m_boardWidth * m_boardHeight);
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

}