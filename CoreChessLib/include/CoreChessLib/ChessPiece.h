#pragma once
#include <string>
#include <CoreLib/Math/Vector2.h>

#include "ChessTypes.h"
#include "Internal/ChessPieceRuleSet.h"

namespace CoreChess::Internal {
	class ChessBoard;
}

namespace CoreChess {

	class ChessPieceRegistry;

	class ChessPiece {
	public:
		~ChessPiece() = default;
	
		void AddMoveRule(int x, int y);
		void AddMoveRule(const Vector2& direction);

		void AddMoveRules(const std::vector<ChessMoveRule>& rules);
		void ClearMoveRules();

		bool IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const;
		bool IsSliding() const;

		ChessPieceID GetID() const;
		const std::string& GetName() const;

		uint16_t GetMaxSteps() const;
		TargetType GetTargetType() const;
		PathMode GetPathMode() const;
		PriorityAxis GetPriorityAxis() const;
		const std::vector<ChessMoveRule>& GetMoveRules() const;

		/*
		* @brief Sets the max amount of steps a piece can move in its direction.
		* 
		* note: if maxStep = 0 -> infinite maxStep
		*/
		void SetMaxSteps(uint16_t maxSteps);

		/*
		* @brief Sets whether a piece can slide (queen, bishop) or jump like (knight)
		*/
		void SetSliding(bool value);

		void SetTargetType(TargetType type);

		/*
		* @brief Sets how the the pieces moves when sliding is turned on
		*/
		void SetPathMode(PathMode pathMode);

		/*
		* @brief Sets in what order the piece should move.
		* 
		* note: only takes effect if path mode is PathMode::AXIS_ORDER
		*/
		void SetPriorityAxis(PriorityAxis axis);

	private:
		friend class ChessPieceRegistry;

		ChessPieceID m_id;
		std::string m_name;

		uint16_t m_maxSteps = 1;
		bool m_sliding = false;
		TargetType m_targetType = TargetType::ANY;
		PathMode m_pathMode = PathMode::LINEAR;
		PriorityAxis m_priorityAxis = PriorityAxis::X;

		Internal::ChessPieceRuleSet m_moveRules;

		ChessPiece(ChessPieceID id, const std::string& name);
	};

}