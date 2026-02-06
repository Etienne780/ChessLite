#include "ChessPiece.h"
#include "Internal/ChessPieceRuleSet.h"

namespace CoreChess {

	void ChessPiece::AddMoveRule(int x, int y) {
		AddMoveRule(Vector2{ static_cast<float>(x), static_cast<float>(y) });
	}

	void ChessPiece::AddMoveRule(const Vector2& direction) {
		m_moveRules.AddRule(direction, m_maxSteps, m_sliding, m_targetType, m_pathMode, m_priorityAxis);
	}

	void ChessPiece::AddMoveRules(const std::vector<ChessMoveRule>& rules) {
		for (const auto& rule : rules) {
			m_moveRules.AddRule(rule);
		}
	}

	void ChessPiece::ClearMoveRules() {
		m_moveRules.ClearRules();
	}

	bool ChessPiece::IsValidMove(const ChessBoard& board, const Vector2& from, const Vector2& to) const {
		return m_moveRules.IsValidMove(board, from, to);
	}

	bool ChessPiece::IsSliding() const {
		return m_sliding;
	}

	ChessPieceID ChessPiece::GetID() const {
		return m_id;
	}

	const std::string& ChessPiece::GetName() const {
		return m_name;
	}

	uint16_t ChessPiece::GetMaxSteps() const {
		return m_maxSteps;
	}

	TargetType ChessPiece::GetTargetType() const {
		return m_targetType;
	}

	PathMode ChessPiece::GetPathMode() const {
		return m_pathMode;
	}

	PriorityAxis ChessPiece::GetPriorityAxis() const {
		return m_priorityAxis;
	}

	const std::vector<ChessMoveRule>& ChessPiece::GetMoveRules() const {
		return m_moveRules.GetRules();
	}

	int ChessPiece::GetMaterialValue() const {
		return m_materialValue;
	}

	void ChessPiece::SetMoveProperties(uint16_t maxSteps, bool sliding, TargetType targetType,
		PathMode pathMode, PriorityAxis priorityAxis) 
	{
		SetMaxSteps(maxSteps);
		SetSliding(sliding);
		SetTargetType(targetType);
		SetPathMode(pathMode);
		SetPriorityAxis(priorityAxis);
	}

	void ChessPiece::SetMaxSteps(uint16_t maxSteps) {
		m_maxSteps = maxSteps;
	}

	void ChessPiece::SetSliding(bool value) {
		m_sliding = value;
	}

	void ChessPiece::SetTargetType(TargetType type) {
		m_targetType = type;
	}

	void ChessPiece::SetPathMode(PathMode pathMode) {
		m_pathMode = pathMode;
	}

	void ChessPiece::SetPriorityAxis(PriorityAxis axis) {
		m_priorityAxis = axis;
	}

	ChessPiece::ChessPiece(ChessPieceID id, const std::string& name, int materialValue)
		: m_id(id), m_name(name), m_materialValue(materialValue) {
	}

}