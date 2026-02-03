#pragma once
#include <memory>
#include <vector>

#include "ChessTypes.h"

namespace CoreChess {

	class ChessPiece;
	
	class ChessPieceRegistry {
	public:
		static ChessPieceRegistry& Instance();

		ChessPiece* AddChessPiece(ChessPieceID& outID, const std::string& name);

	private:
		CoreChessIDManager m_idManager;
		std::vector<std::unique_ptr<ChessPiece>> m_pieces;

		ChessPieceRegistry();
	};

}