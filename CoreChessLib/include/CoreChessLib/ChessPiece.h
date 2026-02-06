#pragma once
#include <string>
#include <CoreLib/Math/Vector2.h>

#include "ChessTypes.h"
#include "Internal/ChessPieceRuleSet.h"

namespace CoreChess {

	class ChessBoard;
	class ChessPieceRegistry;


	/**
	* @brief Defines a chess piece and its movement rules.
	*
	* A ChessPiece describes how a piece is allowed to move on the board.
	* It does not store board state or position, only rule definitions
	* such as movement directions, step limits, sliding behavior, and
	* target constraints.
	*
	* Instances are created and managed by the ChessPieceRegistry.
	*/
	class ChessPiece {
	public:
		~ChessPiece() = default;

		/**
		* @brief Adds a movement rule using integer direction components.
		*
		* The direction defines the relative movement vector for this rule.
		*
		* @param x X-axis direction.
		* @param y Y-axis direction.
		*/
		void AddMoveRule(int x, int y);
		/**
		* @brief Adds a movement rule using a direction vector.
		*
		* The direction defines the relative movement vector for this rule.
		*
		* @param direction Movement direction vector.
		*/
		void AddMoveRule(const Vector2& direction);
		/**
		* @brief Adds multiple movement rules at once.
		*
		* @param rules List of move rules to add.
		*/
		void AddMoveRules(const std::vector<ChessMoveRule>& rules);

		/**
		* @brief Removes all movement rules from this piece.
		*/
		void ClearMoveRules();

		/**
		* @brief Checks whether a move is valid according to this piece's rules.
		*
		* This function evaluates all registered movement rules and verifies
		* movement pattern, path blocking, and target constraints.
		*
		* @param board Reference to the chess board.
		* @param from  Source position.
		* @param to    Target position.
		* @return True if the move is valid, false otherwise.
		*/
		bool IsValidMove(const ChessBoard& board, const Vector2& from, const Vector2& to) const;

		/**
		* @brief Returns whether this piece uses sliding movement.
		*/
		bool IsSliding() const;

		/**
		* @brief Returns the unique identifier of this chess piece.
		*/
		ChessPieceID GetID() const;

		/**
		* @brief Returns the display name of this chess piece.
		*/
		const std::string& GetName() const;

		/**
		* @brief Returns the maximum number of steps per move.
		*/
		uint16_t GetMaxSteps() const;

		/**
		* @brief Returns the allowed target type for this piece.
		*/
		TargetType GetTargetType() const;

		/**
		* @brief Returns the path evaluation mode used by this piece.
		*/
		PathMode GetPathMode() const;

		/**
		* @brief Returns the priority axis for axis-ordered movement.
		*/
		PriorityAxis GetPriorityAxis() const;

		/**
		* @brief Returns all movement rules associated with this piece.
		*/
		const std::vector<ChessMoveRule>& GetMoveRules() const;

		int GetMaterialValue() const;

		/**
		* @brief Configures all movement-related properties of the chess piece at once.
		*
		* This is a convenience function that updates all movement parameters
		* in a single call. Internally, it forwards the values to the corresponding
		* individual setter functions.
		*
		* @param maxSteps      Maximum number of steps per move (0 = unlimited).
		* @param sliding       Enables sliding movement if true.
		* @param targetType    Defines which target fields are allowed.
		* @param pathMode      Defines how paths are evaluated when sliding.
		* @param priorityAxis Axis priority for PathMode::AXIS_ORDER.
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
		int m_materialValue = 0;

		uint16_t m_maxSteps = 1;
		bool m_sliding = false;
		TargetType m_targetType = TargetType::ANY;
		PathMode m_pathMode = PathMode::LINEAR;
		PriorityAxis m_priorityAxis = PriorityAxis::X;

		Internal::ChessPieceRuleSet m_moveRules;

		ChessPiece(ChessPieceID id, const std::string& name, int materialValue);
	};

}