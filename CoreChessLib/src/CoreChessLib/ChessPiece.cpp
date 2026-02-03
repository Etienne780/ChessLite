#include "ChessPiece.h"
#include "Internal/ChessPieceRuleSet.h"

namespace CoreChess {

	ChessPieceID ChessPiece::GetID() const {
		return m_id;
	}

	const std::string& ChessPiece::GetName() const {
		return m_name;
	}

	bool ChessPiece::IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const {
		if (!m_rules)
			return false;
		
		return m_rules->IsValidMove(board, from, to);
	}

	ChessPiece::ChessPiece(ChessPieceID id, const std::string& name)
		: m_id(id), m_name(name) {
	}

}