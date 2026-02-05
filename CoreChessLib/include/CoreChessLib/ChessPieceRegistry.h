#pragma once
#include <memory>
#include <vector>

#include "ChessTypes.h"

namespace CoreChess {

	class ChessPiece;
	
	class ChessPieceRegistry {
	public:
		ChessPieceRegistry(const ChessPieceRegistry&) = delete;
		ChessPieceRegistry& operator=(const ChessPieceRegistry&) = delete;

		static ChessPieceRegistry& GetInstance();

		ChessPiece* AddChessPiece(ChessPieceID& outID, const std::string& name);

		std::vector<ChessPiece*> GetAllPieces() const;
	private:
		CoreChessIDManager m_idManager;
		std::vector<std::unique_ptr<ChessPiece>> m_regPieces;

		ChessPieceRegistry();
	};

}