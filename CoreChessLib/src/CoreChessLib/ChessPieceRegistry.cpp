#include "ChessPieceRegistry.h"
#include "ChessPiece.h"

namespace CoreChess {

	ChessPieceRegistry& ChessPieceRegistry::GetInstance() {
		static ChessPieceRegistry reg;
		return reg;
	}

	ChessPiece* ChessPieceRegistry::AddChessPiece(ChessPieceID& outID, const std::string& name, int materialValue) {
		ChessPieceID id(m_idManager.GetNewUniqueIdentifier());
		ChessPiece piece(id, name, materialValue);
		auto ptr = std::make_unique<ChessPiece>(piece);

		outID = id;
		m_regPieces.emplace_back(std::move(ptr));
		return m_regPieces.back().get();
	}

	const ChessPiece* ChessPieceRegistry::GetChessPiece(ChessPieceID id) const {
		for (const auto& piece : m_regPieces) {
			if (piece->GetID() == id) {
				return piece.get();
			}
		}
		return nullptr;
	}

	std::vector<ChessPiece*> ChessPieceRegistry::GetAllPieces() const {
		std::vector<ChessPiece*> result;
		result.reserve(m_regPieces.size());

		for (const auto& piece : m_regPieces) {
			if(piece.get())
				result.push_back(piece.get());
		}

		return result;
	}

}