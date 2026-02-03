#pragma once
#include <CoreLib/Math/Vector2.h>

namespace CoreChess::Internal {
	
	class ChessBoard;

	class ChessRule {
	public:
		bool IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const;

	private:

	};

}