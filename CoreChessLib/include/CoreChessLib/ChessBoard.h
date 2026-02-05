#pragma once
#include <CoreLib/Math/Vector2.h>

#include "CoreChessLib/ChessTypes.h"
#include "CoreChessLib/ChessField.h"

namespace CoreChess {

	class ChessBoard {
	public:
		ChessBoard() = default;
		ChessBoard(int w, int h);
		~ChessBoard() = default;

		void MovePieceFromTo(const Vector2& from, const Vector2& to);
		void MovePieceFromTo(int fromX, int fromY, int toX, int toY);
		void SetFieldAt(const Vector2& at, FieldType type, ChessPieceID id = ChessPieceID(CORE_CHESS_INVALID_ID));
		void SetFieldAt(int atX, int atY, FieldType type, ChessPieceID id = ChessPieceID(CORE_CHESS_INVALID_ID));

		bool HasPieceAt(const Vector2& pos) const;
		bool HasPieceAt(int x, int y) const;

		bool IsPieceAtEqual(const Vector2& pos, FieldType type) const;
		bool IsPieceAtEqual(int x, int y, FieldType type) const;

		ChessField GetFieldAt(const Vector2& pos) const;
		ChessField GetFieldAt(int x, int y) const;

	private:
		int m_boardWidth = 0;
		int m_boardHeight = 0;
		std::vector<ChessField> m_board;

		static std::vector<ChessField> GenerateBoard(int width, int height);
		size_t PosToIndex(int x, int y) const;
	};

}