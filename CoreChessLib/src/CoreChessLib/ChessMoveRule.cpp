#include <numeric>

#include "ChessMoveRule.h"
#include "ChessBoard.h"

namespace CoreChess {

	ChessMoveRule::ChessMoveRule(const Vector2& dir, uint16_t maxSteps, bool slide, TargetType targetType, PathMode path, PriorityAxis axis)
		: m_direction(dir), m_maxSteps(maxSteps), m_slide(slide), m_targetType(targetType), m_pathMode(path), m_priorityAxis(axis) {
	}

	bool ChessMoveRule::IsValidMove(const ChessBoard& board, const Vector2& from, const Vector2& to) const {
		bool isWhite = board.IsPieceAtEqual(from, FieldType::WHITE);
		Vector2 dir = (isWhite) ? from - to : to - from;

		if (!IsMovePatternValid(dir))
			return false;

		if (m_slide) {
			std::vector<Vector2> path = ComputePath(from, to);

			for(const auto& point : path) {
				if (board.HasPieceAt(point))
					return false;
			}
		}

		return IsTargetValid(board, from, to);
	}

	const Vector2& ChessMoveRule::GetDirection() const {
		return m_direction;
	}

	uint16_t ChessMoveRule::GetMaxSteps() const {
		return m_maxSteps;
	}

	bool ChessMoveRule::GetSliding() const {
		return m_slide;
	}

	TargetType ChessMoveRule::GetTargetType() const {
		return m_targetType;
	}

	PathMode ChessMoveRule::GetPathMode() const {
		return m_pathMode;
	}

	PriorityAxis ChessMoveRule::GetPriorityAxis() const {
		return m_priorityAxis;
	}

	void ChessMoveRule::SetDirection(const Vector2& dir) {
		m_direction = dir;
	}

	void ChessMoveRule::SetMaxSteps(uint16_t maxSteps) {
		m_maxSteps = maxSteps;
	}

	void ChessMoveRule::SetSliding(bool value) {
		m_slide = value;
	}

	void ChessMoveRule::SetTargetType(TargetType targetType) {
		m_targetType = targetType;
	}

	void ChessMoveRule::SetPathMode(PathMode path) {
		m_pathMode = path;
	}

	void ChessMoveRule::SetPriorityAxis(PriorityAxis axis) {
		m_priorityAxis = axis;
	}

	bool ChessMoveRule::IsMovePatternValid(const Vector2& dir) const {
		if (dir.x == 0 && dir.y == 0)
			return false;

		const int dx = static_cast<int>(m_direction.x);
		const int dy = static_cast<int>(m_direction.y);
		const int x = static_cast<int>(dir.x);
		const int y = static_cast<int>(dir.y);

		// Handle vertical direction
		if (dx == 0) {
			if (x != 0)
				return false;

			int k = y / dy;
			if (k <= 0)
				return false;

			return m_maxSteps == 0 || k <= m_maxSteps;
		}

		// General case
		if (x % dx != 0)
			return false;

		int k = x / dx;

		if (k <= 0)
			return false;

		if (y != dy * k)
			return false;

		return m_maxSteps == 0 || k <= m_maxSteps;
	}

	bool ChessMoveRule::IsTargetValid(const ChessBoard& board, const Vector2& from, const Vector2& to) const {
		ChessField target = board.GetFieldAt(to);
		FieldType pieceType = board.GetFieldAt(from).GetFieldType();

		switch (m_targetType) {
		case CoreChess::TargetType::ANY:
			// Any target except own piece
			return target.IsPieceNone() || target.GetFieldType() != pieceType;

		case CoreChess::TargetType::FREE:
			return target.IsPieceNone();

		case CoreChess::TargetType::OPPONENT:
			return !target.IsPieceNone() && target.GetFieldType() != pieceType;

		default:
			return false;
		}
	}

	std::vector<Vector2> ChessMoveRule::ComputePath(const Vector2& from, const Vector2& to) const {
		switch (m_pathMode) {
		case CoreChess::PathMode::LINEAR:
			return ComputeLinearPath(from, to);

		case CoreChess::PathMode::AXIS_ORDER:
			return ComputeAxisOrderPath(from, to);

		default:
			return {};
		}
	}

	std::vector<Vector2> ChessMoveRule::ComputeLinearPath(const Vector2& from, const Vector2& to) const {
		Vector2 delta = to - from;

		int absX = std::abs(static_cast<int>(delta.x));
		int absY = std::abs(static_cast<int>(delta.y));
		int gcd = std::gcd(absX, absY);

		if (gcd == 0)
			return {};

		float stepX = std::floor(delta.x / gcd);
		float stepY = std::floor(delta.y / gcd);

		Vector2 step{ stepX, stepY };

		std::vector<Vector2> result;
		result.reserve(gcd);

		Vector2 current = from;
		for (int i = 0; i < gcd; ++i) {
			current += step;
			result.push_back(current);
		}

		// Remove the target field; capture logic is handled elsewhere
		result.pop_back();
		return result;
	}

	std::vector<Vector2> ChessMoveRule::ComputeAxisOrderPath(const Vector2& from, const Vector2& to) const {
		Vector2 delta = to - from;

		int stepX = (delta.x > 0) - (delta.x < 0);
		int stepY = (delta.y > 0) - (delta.y < 0);

		int absX = std::abs(static_cast<int>(delta.x));
		int absY = std::abs(static_cast<int>(delta.y));

		std::vector<Vector2> result;
		result.reserve(static_cast<size_t>(absX + absY));

		Vector2 current = from;

		auto advance = [&](int count, int axis) {
			for (int i = 0; i < count; ++i) {
				current[axis] += (axis == 0 ? stepX : stepY);
				result.push_back(current);
			}
		};

		if (m_priorityAxis == PriorityAxis::X) {
			advance(absX, 0);
			advance(absY, 1);
		}
		else {
			advance(absY, 1);
			advance(absX, 0);
		}

		// Remove the target field; capture logic is handled elsewhere
		result.pop_back();
		return result;
	}

}