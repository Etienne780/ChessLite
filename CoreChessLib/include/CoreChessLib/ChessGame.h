#pragma once
#include <cstdint>

namespace ChessCore {

	enum class ChessGameState : uint16_t {
		IDLE = 0,	
		PLAYING,
		BLACK_WON,
		WHITE_WON
	};

	class ChessGame {
	public:
		ChessGame();

		void StartGame();
		void EndGame();

		bool IsGameEnd();

		void MovePiece();

	private:
		ChessGameState m_gameState = ChessGameState::IDLE;
	};

}