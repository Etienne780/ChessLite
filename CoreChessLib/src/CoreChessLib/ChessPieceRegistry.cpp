#include "ChessPieceRegistry.h"
#include "ChessPiece.h"

namespace CoreChess {

	ChessPieceRegistry& ChessPieceRegistry::Instance() {
		static ChessPieceRegistry reg;
		return reg;
	}

	ChessPiece* ChessPieceRegistry::AddChessPiece(ChessPieceID & outID, const std::string& name) {
		ChessPieceID id(m_idManager.GetNewUniqueIdentifier());
		ChessPiece piece(id, name);
		auto ptr = std::make_unique<ChessPiece>(piece);

		m_pieces.emplace_back(std::move(ptr));
		return m_pieces.back().get();
	}

	ChessPieceRegistry::ChessPieceRegistry() {
	}

}