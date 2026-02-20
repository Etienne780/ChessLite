#include <cmath>
#include "AI/Agent.h"

Agent::Agent(const std::string& name, const CoreChess::ChessContext& context)
	: m_name(name), m_chessConfigString(context.GetConfigString()) {
}

const GameMove& Agent::GetBestMove(const CoreChess::ChessGame& game) {
	if (m_gameFinished) {
		m_moveHistory.clear();
		m_gameFinished = false;

		m_isWhite = game.IsWhiteTurn();
	}
	
	if (game.GetContext().GetConfigString() != m_chessConfigString) {
		throw std::runtime_error("Agent: Unsupported game configuration");
	}

	const auto& board = game.GetBoard();
	std::string state = GetNormalizedBoardStr(board, game.IsWhiteTurn());

	BoardState* boardStatePtr = nullptr;

	auto it = m_boardStates.find(state);
	if (it != m_boardStates.end()) {
		boardStatePtr = &it->second;
	}
	else {
		BoardState newState{ state, game };
		boardStatePtr = &m_boardStates.emplace(state, std::move(newState)).first->second;
	}

	float explorationChance = GetExplorationChance();
	size_t moveIndex = boardStatePtr->GetBestMove(explorationChance);

	m_moveHistory.emplace_back(state, moveIndex);
	return boardStatePtr->GetMove(moveIndex);
}

void Agent::GameFinished(bool won) {
	float reward = won ? 1.0f : -1.0f;
	float reductionAmount = 0.2f;
	float currentReward = reward;

	for (auto itHistory = m_moveHistory.rbegin(); itHistory != m_moveHistory.rend(); ++itHistory) {
		auto& [stateKey, moveIndex] = *itHistory;

		auto it = m_boardStates.find(stateKey);
		if (it != m_boardStates.end()) {
			auto& move = it->second.GetMove(moveIndex);
			move.AddEvaluation(currentReward);
		}

		currentReward *= reductionAmount;
	}

	m_gameFinished = true;
	
	m_matchesPlayed++;
	if (won)
		m_matchesWon++;
	if(m_isWhite)
		m_matchesPlayedAsWhite++;
	if (won && m_isWhite)
		m_matchesWonAsWhite++;
}

AgentID Agent::GetID() const {
	return m_id;
}

const std::string& Agent::GetName() const {
	return m_name;
}

const std::string& Agent::GetChessConfig() const {
	return m_chessConfigString;
}

int Agent::GetMatchesPlayed() const {
	return m_matchesPlayed;
}

int Agent::GetWonMatches() const {
	return m_matchesWon;
}

int Agent::GetLostMatches() const {
	return m_matchesPlayed - m_matchesWon;
}

int Agent::GetMatchesPlayedAsWhite() const {
	return m_matchesPlayedAsWhite;
}

int Agent::GetMatchesPlayedAsBlack() const {
	return m_matchesPlayed - m_matchesPlayedAsWhite;
}

int Agent::GetMatchesWonAsWhite() const {
	return m_matchesWonAsWhite;
}

int Agent::GetMatchesWonAsBlack() const {
	return m_matchesWon - m_matchesWonAsWhite;
}

int Agent::GetMatchesLostAsWhite() const {
	return GetMatchesWonAsBlack();
}

int Agent::GetMatchesLostAsBlack() const {
	return GetMatchesWonAsWhite();
}

float Agent::GetExplorationChance() const {
	// hits null at ca 15 games played
	double y = -0.005 * std::pow(m_matchesPlayed, 2) + 1;
	return static_cast<float>(std::max(0.0, y));
}

const std::unordered_map<std::string, BoardState>& Agent::GetNormilzedBoardStates() const {
	return m_boardStates;
}

const std::vector<std::pair<std::string, size_t>>& Agent::GetMoveHistory() const {
	return m_moveHistory;
}

std::string Agent::GetNormalizedBoardStr(const CoreChess::ChessBoard& board, bool isWhite) {
	size_t fieldCount = board.GetNumberOfFields();
	std::string result;
	result.reserve(fieldCount);

	for (size_t i = 0; i < fieldCount; i++) {
		auto field = board.GetFieldAt(i);

		switch (field.GetFieldType()) {
		case CoreChess::FieldType::NONE:
			result += '0';
			break;
		case CoreChess::FieldType::WHITE:
			result += (isWhite) ? '1' : '2';
			break;
		case CoreChess::FieldType::BLACK:
			result += (isWhite) ? '2' : '1';
			break;
		default:
			break;
		}
	}

	return result;
}

void Agent::SetID(AgentID id) {
	m_id = id;
}