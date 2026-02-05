#pragma once
#include <cstdint>
#include <CoreLib/Math/Vector2.h>

#include "ChessTypes.h"
#include "ChessContext.h"

namespace CoreChess {

	class ChessGame {
	public:
		ChessGame() = default;
		ChessGame(const ChessContext& ctx);

		void StartGame();
		void EndGame();

		bool IsGameEnd();

		void MovePiece(int pieceIndex, const Vector2& to);

		void SetGameContext(const ChessContext& ctx);

	private:
		bool m_isContextSet = false;
		ChessContext m_gameContext;
		ChessGameState m_gameState = ChessGameState::IDLE;

		ChessBoard m_board;

		bool CheckContext(const std::string& funcName) const;
	};

}