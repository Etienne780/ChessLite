#pragma once
#include <memory>
#include <vector>

#include "ChessTypes.h"

namespace CoreChess {

	class ChessPiece;
	
	/**
	* @brief Global registry for all chess piece definitions.
	*
	* The ChessPieceRegistry is responsible for creating and storing
	* all chess piece types (e.g. pawn, rook, bishop).
	* Each registered piece receives a unique ChessPieceID and can be
	* reused across multiple games or contexts.
	*
	* This class follows the singleton pattern and cannot be copied.
	*/
	class ChessPieceRegistry {
	public:
		ChessPieceRegistry(const ChessPieceRegistry&) = delete;
		ChessPieceRegistry& operator=(const ChessPieceRegistry&) = delete;

		/**
		* @brief Returns the global instance of the ChessPieceRegistry.
		*
		* @return Reference to the singleton registry instance.
		*/
		static ChessPieceRegistry& GetInstance();

		/**
		* @brief Creates and registers a new chess piece definition.
		*
		* A unique ChessPieceID is generated internally and assigned
		* to the new piece. The created piece is owned by the registry.
		*
		* @param outID Reference that will receive the generated piece ID.
		* @param name  Human-readable name of the chess piece.
		* @param materialValue value of the figure (default = 0)
		* @return Pointer to the newly created ChessPiece. Should not be stored
		*/
		ChessPiece* AddChessPiece(ChessPieceID& outID, const std::string& name, int materialValue = 0);

		/**
		* @brief Returns a registered chess piece by its ID.
		*
		* Searches the registry for a chess piece with the given ChessPieceID.
		*
		* @param id The unique identifier of the chess piece.
		* @return Pointer to the ChessPiece if found, otherwise nullptr. Should not be stored
		*/
		const ChessPiece* GetChessPiece(ChessPieceID id) const;

		/**
		* @brief Returns all registered chess pieces.
		*
		* The returned pointers are non-owning and remain valid as long
		* as the registry exists.
		*
		* @return Vector of pointers to all registered ChessPiece objects.
		*/
		std::vector<ChessPiece*> GetAllPieces() const;
	private:
		CoreChessIDManager m_idManager;
		std::vector<std::unique_ptr<ChessPiece>> m_regPieces;

		ChessPieceRegistry() = default;
	};

}