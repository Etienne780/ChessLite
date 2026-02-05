#pragma once
#include <CoreLib/Math/Vector2.h>
#include "ChessTypes.h"

namespace CoreChess::Internal {
	class ChessBoard;
}

namespace CoreChess {

	class ChessMoveRule {
	public:
		ChessMoveRule(const Vector2& dir, uint16_t maxSteps, bool slide, TargetType targetType, PathMode path = PathMode::LINEAR, PriorityAxis axis = PriorityAxis::X);

		/**
		* @brief Validates whether a move from one position to another is allowed by this move rule.
		*
		* This function checks:
		* 
		* - Whether the move direction and distance match the configured move pattern
		* 
		* - Whether the path is unobstructed if the rule is marked as sliding
		*
		* The destination field itself is NOT checked for occupancy.
		* 
		* Capture logic and target-field validation are handled elsewhere.
		*
		* @param board Reference to the chess board for occupancy checks.
		* @param from Starting position of the move.
		* @param to Target position of the move.
		* @return true if the move is valid according to this rule, false otherwise.
		*/
		bool IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const;

		const Vector2& GetDirection() const;
		uint16_t GetMaxSteps() const;
		bool GetSliding() const;
		TargetType GetTargetType() const;
		PathMode GetPathMode() const;
		PriorityAxis GetPriorityAxis() const;

		void SetDirection(const Vector2& dir);
		void SetMaxSteps(uint16_t maxSteps);
		void SetSliding(bool value);
		void SetTargetType(TargetType targetType);
		void SetPathMode(PathMode path);
		void SetPriorityAxis(PriorityAxis axis);

	private:
		/*
		* @brief Move direction
		*/
		Vector2 m_direction{0, 0};

		/*
		* @brief How often the it is possible to move in m_direction (m_maxSteps * m_direction)
		* 
		* note: if maxStep = 0 -> infinite maxStep
		*/
		uint16_t m_maxSteps = 1;

		/*
		* @brief Indicates if the piece gets blocked by other figures if there in the way
		*/
		bool m_slide = false;

		TargetType m_targetType = TargetType::ANY;

		/*
		* @brief Defines how the movement path between two positions is constructed.
		*
		* LINEAR:
		* 
		*   The path follows the normalized direction vector, moving all axes simultaneously.
		* 
		*   Typical for sliding pieces like rook, bishop, or queen.
		*
		* AXIS_ORDER:
		* 
		*   The path is constructed by moving along one axis first, then the other.
		* 
		*   The order is defined by m_priorityAxis.
		* 
		*   Useful for non-linear or special movement rules.
		*/
		PathMode m_pathMode = PathMode::LINEAR;

		/*
		* @brief Indicates which axis moves first, is important for slide
		*/
		PriorityAxis m_priorityAxis = PriorityAxis::X;

		bool IsMovePatternValid(const Vector2& dir) const;
		bool IsTargetValid(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const;
		std::vector<Vector2> ComputePath(const Vector2& from, const Vector2& to) const;

		std::vector<Vector2> ComputeLinearPath(const Vector2& from, const Vector2& to) const;
		std::vector<Vector2> ComputeAxisOrderPath(const Vector2& from, const Vector2& to) const;
	};

}