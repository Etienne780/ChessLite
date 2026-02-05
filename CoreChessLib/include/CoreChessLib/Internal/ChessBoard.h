#pragma once
#include <CoreLib/Math/Vector2.h>

#include "CoreChessLib/ChessTypes.h"

namespace CoreChess::Internal {

	enum class FieldType : uint32_t{
		NONE = 0,
		WHITE,
		BLACK
	};

	class Field {
	public:
		Field() = default;
		Field(ChessPieceID piece, FieldType type);
		~Field() = default;

		bool IsPieceNone() const;
		bool IsPieceWhite() const;
		bool IsPieceBlack() const;

		ChessPieceID GetPieceID() const;
		FieldType GetFieldType() const;

	private:
		ChessPieceID m_piece;
		FieldType m_type = FieldType::NONE;
	};

	class ChessBoard {
	public:
		ChessBoard(int x, int y);
		~ChessBoard() = default;

		bool HasPieceAt(const Vector2& pos) const;
		bool HasPieceAt(int x, int y) const;

		Field GetFieldAt(const Vector2& pos) const;
		Field GetFieldAt(int x, int y) const;

	private:
		int m_boardSizeX = 0;
		int m_boardSizeY = 0;
		std::vector<Field> m_board;

		size_t PosToIndex(int x, int y) const;
	};

}