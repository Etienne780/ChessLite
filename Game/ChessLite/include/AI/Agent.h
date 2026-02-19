#pragma once
#include <string>
#include <unordered_map>

#include <CoreChessLib/ChessGame.h>

#include "BoardState.h"

class Agent {
public:
	Agent() = default;
	Agent(const std::string& name, const CoreChess::ChessContext& context);
	~Agent() = default;

	const GameMove& GetBestMove(const CoreChess::ChessGame& game);

	void GameFinished(bool won);

	const std::string& GetName() const;
	const std::string& GetChessConfig() const;
	int GetMatchesPlayed() const;
	int GetWonMatches() const;
	int GetLostMatches() const;
	int GetMatchesPlayedAsWhite() const;
	int GetMatchesPlayedAsBlack() const;
	float GetExplorationChance() const;
	const std::unordered_map<std::string, BoardState>& GetNormilzedBoardStates() const;
	const std::vector<std::pair<std::string, size_t>>& GetMoveHistory() const;

private:
	bool m_gameFinished = false;
	std::string m_name = "UNKOWN";
	int m_matchesPlayed = 0;
	int m_matchesWon = 0;
	int m_matchesPlayedAsWhite = 0;
	bool m_isWhite = false;
	std::string m_chessConfigString;
	std::unordered_map<std::string, BoardState> m_boardStates;/* < normalized board position to board state*/

	std::vector<std::pair<std::string, size_t>> m_moveHistory;/* < board state, move index*/
	
	std::string GetNormalizedBoardStr(const CoreChess::ChessBoard& board, bool isWhite);
};