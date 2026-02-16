#pragma once
#include <CoreChessLib/ChessGame.h>

#include "GameMove.h"

class BoardState {
public:
	BoardState() = default;
	BoardState(const std::string& state, const CoreChess::ChessGame& game);
	~BoardState() = default;

	int GetBestMove(float explorationChance = 0.1f);

	GameMove& GetMove(int index);
	Vector2 GetMoveTo(int index) const;

	const std::string& GetState() const;
	const std::vector<GameMove> GetPossibleMoves() const;

private:
	std::string m_state;
	std::vector<GameMove> m_possibleMoves;

	std::vector<GameMove> CalculatePossibleMoves(const CoreChess::ChessGame& game);
 };