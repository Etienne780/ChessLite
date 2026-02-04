#pragma once
#include "ChessTypes.h"

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

		ChessPieceID GetPieceID() const;
		FieldType GetFieldType() const;

		bool IsPieceNone() const;
		bool IsPieceWhite() const;
		bool IsPieceBlack() const;

	private:
		ChessPieceID m_piece;
		FieldType m_type = FieldType::NONE;
	};

	class ChessBoard {
	public:
		ChessBoard(int x, int y);

	private:
		int m_boardSizeX = 0;
		int m_boardSizeY = 0;
		std::vector<Field> m_board;
	};

}