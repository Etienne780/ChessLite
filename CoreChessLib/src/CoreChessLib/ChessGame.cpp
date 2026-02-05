#include <CoreLib/Log.h>

#include "ChessGame.h"
#include "ChessContext.h"

namespace CoreChess {

	ChessGame::ChessGame(const ChessContext& ctx) 
		: m_gameContext(ctx), m_isContextSet(true) {
	}

	void ChessGame::StartGame() {
		if (!CheckContext("StartGame"))
			return;

		if (m_gameState == ChessGameState::PLAYING) {
			Log::Error("CoreChess::StartGame: Faild to StartGame, a game is already running!");
			return;
		}
	}

	void ChessGame::EndGame() {
		if (!CheckContext("EndGame"))
			return;
	}

	bool ChessGame::IsGameEnd() {
		return false;
	}

	void ChessGame::MovePiece(int pieceIndex, const Vector2& to) {
		if (!CheckContext("EndGame"))
			return;
	}

	void ChessGame::SetGameContext(const ChessContext& ctx) {
		m_isContextSet = true;
		m_gameContext = ctx;
	}

	bool ChessGame::CheckContext(const std::string& funcName) const {
		if (!m_isContextSet) {
			Log::Error("CoreChess::{}: Faild to {}, Chess context was not set!", funcName);
			return false;
		}
		return true;
	}

}