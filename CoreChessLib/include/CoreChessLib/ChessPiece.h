#pragma once
#include <string>
#include <CoreLib/Math/Vector2.h>

#include "ChessTypes.h"

namespace CoreChess::Internal {
	class ChessPieceRuleSet;
	class ChessBoard;
}

namespace CoreChess {

	class ChessPieceRegistry;

	class ChessPiece {
	public:
		~ChessPiece() = default;
	
		ChessPieceID GetID() const;
		const std::string& GetName() const;

		bool IsValidMove(const Internal::ChessBoard& board, const Vector2& from, const Vector2& to) const;

	private:
		friend class ChessPieceRegistry;

		ChessPieceID m_id;
		std::string m_name;

		std::unique_ptr<Internal::ChessPieceRuleSet> m_rules;

		ChessPiece(ChessPieceID id, const std::string& name);
	};

}