#pragma once
#include <string>
#include <CoreLib/Math/Vector2.h>

#include "ChessTypes.h"
#include "Internal/ChessPieceRuleSet.h"

namespace CoreChess {

	class ChessBoard;
	class ChessPieceRegistry;

	class ChessPiece {
	public:
		~ChessPiece() = default;

		void AddMoveRule(int x, int y);
		void AddMoveRule(const Vector2& direction);

		void AddMoveRules(const std::vector<ChessMoveRule>& rules);
		void ClearMoveRules();

		bool IsValidMove(const ChessBoard& board, const Vector2& from, const Vector2& to) const;
		bool IsSliding() const;

		ChessPieceID GetID() const;
		const std::string& GetName() const;

		uint16_t GetMaxSteps() const;
		TargetType GetTargetType() const;
		PathMode GetPathMode() const;
		PriorityAxis GetPriorityAxis() const;
		const std::vector<ChessMoveRule>& GetMoveRules() const;

		/**
		* @brief Configures all movement-related properties of the chess piece at once.
		*
		* This function is a convenience wrapper that sets all move rule parameters
		* in a single call. Internally, it forwards the values to the corresponding
		* individual setter functions.
		*
		* @param maxSteps        Maximum number of steps the piece can move in its direction.
		*                        A value of 0 means unlimited range.
		* @param sliding         Whether the piece moves by sliding (e.g. rook, bishop, queen)
		*                        or by jumping (e.g. knight).
		* @param targetType      Defines what kind of target field is allowed
		*                        (free, opponent, or any).
		* @param pathMode        Defines how the path is evaluated when sliding is enabled.
		* @param priorityAxis   Defines the movement order of axes when using
		*                        PathMode::AXIS_ORDER.
		*/
		void SetMoveProperties(uint16_t maxSteps, bool sliding, TargetType targetType, 
			PathMode pathMode = PathMode::LINEAR, PriorityAxis priorityAxis = PriorityAxis::X);

		/*
		* @brief Sets the maximum number of steps the piece can move in its direction.
		*
		* @note If maxSteps is 0, the piece can move infinitely in that direction.
		*/
		void SetMaxSteps(uint16_t maxSteps);

		/*
		* @brief Enables or disables sliding movement.
		*
		* Sliding pieces (e.g. rook, bishop, queen) are blocked by other pieces.
		* Non-sliding pieces (e.g. knight) can jump over other pieces.
		*/
		void SetSliding(bool value);

		/*
		* @brief Defines what kind of target field this move rule allows.
		*
		* Example: free field only, opponent piece only, or both.
		*/
		void SetTargetType(TargetType type);

		/*
		* @brief Sets how the piece moves along its path when sliding is enabled.
		*
		* Example: linear movement or axis-ordered movement.
		*/
		void SetPathMode(PathMode pathMode);

		/*
		* @brief Sets the axis movement order for axis-ordered paths.
		*
		* @note Only used when path mode is PathMode::AXIS_ORDER.
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