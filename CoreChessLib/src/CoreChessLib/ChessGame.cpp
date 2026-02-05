#include "ChessGame.h"
#include "ChessContext.h"

namespace CoreChess {

	ChessGame::ChessGame(const ChessContext& ctx) 
		: m_gameContext(ctx), m_isContextSet(true) {
	}

	void ChessGame::StartGame() {
		
	}

	void ChessGame::EndGame() {

	}

	bool ChessGame::IsGameEnd() {
		return false;
	}

	void ChessGame::MovePiece(int pieceIndex, const Vector2& to) {
		
	}

	void ChessGame::SetGameContext(const ChessContext& ctx) {
		m_isContextSet = true;
		m_gameContext = ctx;
	}

}