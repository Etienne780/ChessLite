#pragma once
#include <vector>
#include <CoreLib/Math/Vector2.h>

#include "Internal/ChessRule.h"


namespace CoreChess::Internal {

	class ChessBoard;

	class ChessPieceRuleSet {
	public:
		bool IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const;

	private:

		std::vector<ChessRule> m_rules;
	};

}