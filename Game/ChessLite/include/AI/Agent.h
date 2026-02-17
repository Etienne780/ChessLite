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
	const std::unordered_map<std::string, BoardState>& GetNormilzedBoardStates() const;
	const std::vector<std::pair<std::string, size_t>>& GetMoveHistory() const;

private:
	bool m_gameFinished = false;
	std::string m_name = "UNKOWN";
	std::string m_chessConfigString;
	std::unordered_map<std::string, BoardState> m_boardStates;/* < normalized board position to board state*/

	std::vector<std::pair<std::string, size_t>> m_moveHistory;/* < board state, move index*/
	
	std::string GetNormalizedBoardStr(const CoreChess::ChessBoard& board, bool isWhite);
};