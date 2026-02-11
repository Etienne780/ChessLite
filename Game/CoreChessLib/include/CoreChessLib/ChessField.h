#pragma once
#include "ChessTypes.h"

namespace CoreChess {

	class ChessField {
	public:
		ChessField() = default;
		ChessField(ChessPieceID piece, FieldType type);
		~ChessField() = default;

		bool IsPieceNone() const;
		bool IsPieceWhite() const;
		bool IsPieceBlack() const;

		ChessPieceID GetPieceID() const;
		FieldType GetFieldType() const;

	private:
		ChessPieceID m_piece;
		FieldType m_type = FieldType::NONE;
	};

}