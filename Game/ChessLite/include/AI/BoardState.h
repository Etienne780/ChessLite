#pragma once
#include <CoreChessLib/ChessGame.h>

#include "GameMove.h"

class BoardState {
public:
	BoardState() = default;
	BoardState(const std::string& state, const CoreChess::ChessGame& game);
	~BoardState() = default;

	size_t GetBestMove(float explorationChance = 0.1f);

	GameMove& GetMove(size_t index);
	const GameMove& GetMove(size_t index) const;
	Vector2 GetMoveTo(size_t index) const;

	const std::string& GetState() const;
	const std::vector<GameMove> GetPossibleMoves() const;

private:
	std::string m_state;
	std::vector<GameMove> m_possibleMoves;

	std::vector<GameMove> CalculatePossibleMoves(const CoreChess::ChessGame& game);
 };