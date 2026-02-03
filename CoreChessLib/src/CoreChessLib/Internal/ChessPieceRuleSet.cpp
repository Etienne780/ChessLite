#include "Internal/ChessPieceRuleSet.h"

namespace CoreChess::Internal {

	bool ChessPieceRuleSet::IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const {
		for (auto& rule : m_rules) {
			if (rule.IsValidMove(board, from, to))
				return true;
		}
		return false;
	}

}