#pragma once
#include <CoreLib/Math/Vector2.h>

#include "CoreChessLib/ChessTypes.h"
#include "CoreChessLib/ChessField.h"

namespace CoreChess::Internal {

	class ChessBoard {
	public:
		ChessBoard() = default;
		ChessBoard(int x, int y);
		~ChessBoard() = default;

		bool HasPieceAt(const Vector2& pos) const;
		bool HasPieceAt(int x, int y) const;

		bool IsPieceAtEqual(const Vector2& pos, FieldType type) const;
		bool IsPieceAtEqual(int x, int y, FieldType type) const;

		ChessField GetFieldAt(const Vector2& pos) const;
		ChessField GetFieldAt(int x, int y) const;

	private:
		int m_boardSizeX = 0;
		int m_boardSizeY = 0;
		std::vector<ChessField> m_board;

		size_t PosToIndex(int x, int y) const;
	};

}