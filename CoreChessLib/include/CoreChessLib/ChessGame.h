#pragma once
#include <cstdint>
#include <CoreLib/Math/Vector2.h>

#include "ChessTypes.h"

namespace CoreChess {

	class ChessGame {
	public:
		ChessGame();

		void StartGame();
		void EndGame();

		bool IsGameEnd();

		void MovePiece(int pieceIndex, const Vector2& to);

	private:
		ChessGameState m_gameState = ChessGameState::IDLE;
	};

}