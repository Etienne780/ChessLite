#include "AI/Agent.h"

Agent::Agent(const std::string& name, const CoreChess::ChessContext& context)
	: m_name(name), m_chessConfigString(context.GetConfigString()) {
}

const GameMove& Agent::GetBestMove(const CoreChess::ChessGame& game) {
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

	float explorationChance = 0.1f;
	size_t moveIndex = boardStatePtr->GetBestMove(explorationChance);

	m_moveHistory.emplace_back(state, moveIndex);
	return boardStatePtr->GetMove(moveIndex);
}

void Agent::GameFinished(bool won) {
	float reward = won ? 1.0f : -1.0f;

	for (auto& [stateKey, moveIndex] : m_moveHistory) {
		auto it = m_boardStates.find(stateKey);
		if (it != m_boardStates.end()) {
			auto& move = it->second.GetMove(moveIndex); 
			move.AddEvaluation(reward);
		}
	}

	m_moveHistory.clear();
}

const std::string& Agent::GetName() const {
	return m_name;
}

const std::string& Agent::GetChessConfig() const {
	return m_chessConfigString;
}

const std::unordered_map<std::string, BoardState>& Agent::GetNormilzedBoardStates() {
	return m_boardStates;
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