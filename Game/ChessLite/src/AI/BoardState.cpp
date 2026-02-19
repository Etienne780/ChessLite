#include <CoreLib/Random.h>
#include <CoreChessLib/ChessPieceRegistry.h>
#include <CoreChessLib/ChessPiece.h>

#include "AI/BoardState.h"

BoardState::BoardState(const std::string& state, 
	const CoreChess::ChessGame& game)
	: m_state(state) {
	m_possibleMoves = CalculatePossibleMoves(game);
}

size_t BoardState::GetBestMove(float explorationChance) {
	if (m_possibleMoves.empty())
		throw std::runtime_error("No moves available");

	if (Random::GetNumber<float>() < explorationChance)
		return Random::GetRangeNumber<size_t>(0, m_possibleMoves.size() - 1);

	float bestScore = m_possibleMoves[0].GetEvaluation();

	for (size_t i = 1; i < m_possibleMoves.size(); i++)
		bestScore = std::max(bestScore, m_possibleMoves[i].GetEvaluation());

	std::vector<size_t> bestMoves;

	for (size_t i = 0; i < m_possibleMoves.size(); i++) {
		if (m_possibleMoves[i].GetEvaluation() == bestScore)
			bestMoves.push_back(i);
	}

	return bestMoves[Random::GetRangeNumber<size_t>(0, bestMoves.size() - 1)];
}

GameMove& BoardState::GetMove(size_t index) {
	return m_possibleMoves.at(index);
}

const GameMove& BoardState::GetMove(size_t index) const {
	return m_possibleMoves.at(index);
}

Vector2 BoardState::GetMoveTo(size_t index) const {
	return m_possibleMoves.at(index).GetTo();
}

const std::string& BoardState::GetState() const {
	return m_state;
}

const std::vector<GameMove>& BoardState::GetPossibleMoves() const {
	return m_possibleMoves;
}

std::vector<GameMove> BoardState::CalculatePossibleMoves(
	const CoreChess::ChessGame& game)
{
	const auto& board = game.GetBoard();
	bool isSelfWhite = game.IsWhiteTurn();
	size_t fieldCount = board.GetNumberOfFields();
	std::vector<GameMove> possibleMoves;

	auto& reg = CoreChess::ChessPieceRegistry::GetInstance();

	for (size_t i = 0; i < fieldCount; i++) {
		auto field = board.GetFieldAt(i);

		if (field.IsPieceNone())
			continue;

		bool isOwnPiece =
			(field.IsPieceWhite() && isSelfWhite) ||
			(field.IsPieceBlack() && !isSelfWhite);

		if (!isOwnPiece)
			continue;

		const auto* piece = reg.GetChessPiece(field.GetPieceID());
		if (!piece)
			continue;

		Vector2 from = board.IndexToPos(i);
		for (size_t j = 0; j < fieldCount; j++) {
			Vector2 to = board.IndexToPos(j);
			
			if (piece->IsValidMove(board, from, to))
				possibleMoves.emplace_back(from, to);
		}
	}

	return possibleMoves;
}