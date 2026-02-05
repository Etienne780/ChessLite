#include "Internal/ChessPieceRuleSet.h"
#include "ChessBoard.h"

namespace CoreChess::Internal {

	void ChessPieceRuleSet::AddRule(const ChessMoveRule& rule) {
		m_rules.push_back(rule);
	}

	void ChessPieceRuleSet::AddRule(const Vector2& direction,
		uint16_t maxSteps,
		bool slide,
		TargetType targetType,
		PathMode pathMode,
		PriorityAxis priorityAxis) {
		m_rules.emplace_back(direction, maxSteps, slide, targetType, pathMode, priorityAxis);
	}

	void ChessPieceRuleSet::ClearRules() {
		m_rules.clear();
	}

	bool ChessPieceRuleSet::IsValidMove(const ChessBoard& board, const Vector2& from, const Vector2& to) const {
		for (auto& rule : m_rules) {
			if (rule.IsValidMove(board, from, to))
				return true;
		}
		return false;
	}

	const std::vector<ChessMoveRule>& ChessPieceRuleSet::GetRules() const {
		return m_rules;
	}

}