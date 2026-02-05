#pragma once
#include <vector>
#include <CoreLib/Math/Vector2.h>

#include "ChessMoveRule.h"


namespace CoreChess::Internal {

	class ChessBoard;

	class ChessPieceRuleSet {
	public:
		ChessPieceRuleSet() = default;
		~ChessPieceRuleSet() = default;

		void AddRule(const ChessMoveRule& rule);
		void AddRule(const Vector2& direction, uint16_t maxSteps, bool slide, TargetType targetType, PathMode pathMode, PriorityAxis priorityAxis);
		void ClearRules();

		bool IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const;

		const std::vector<ChessMoveRule>& GetRules() const;

	private:
		std::vector<ChessMoveRule> m_rules;
	};

}