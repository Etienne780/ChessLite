#pragma once
#include <vector>
#include <unordered_map>

#include "ChessTypes.h"

namespace CoreChess {

	class ChessBoard;

	/**
	* @brief Stores data for a chess game, including pieces and board setup commands.
	*
	* This class is used to define the initial configuration of a chess game,
	* including board size, piece types, and custom placement of pieces.
	*/
	class ChessContext {
	public:
		ChessContext() = default;

		/**
		* @brief Generates a ChessBoard based on the current context setup.
		*
		* Applies all stored board commands to produce a fully initialized board.
		*
		* @return ChessBoard The board with all pieces placed according to the context.
		*/
		ChessBoard GenerateBoard();

		/**
		* @brief Pre-allocates memory for a number of pieces.
		*
		* @param amount Number of pieces to reserve space for.
		* @return Reference to this context for chaining.
		*/
		ChessContext& ReservePiece(size_t amount);

		/**
		* @brief Adds a unique piece type to the context.
		*
		* Duplicate piece IDs will be ignored.
		*
		* @param pieceID ID of the piece type to add.
		* @return Reference to this context for chaining.
		*/
		ChessContext& AddPiece(ChessPieceID pieceID);

		/**
		* @brief Clears all previously defined board setup commands.
		*
		* @return Reference to this context for chaining.
		*/
		ChessContext& ClearBoardSetupCommands();

		ChessContext& ReserveBoardCommands(size_t amount);

		/**
		* @brief Fills an entire row with a single piece type.
		*
		* @param rowIndex Index of the row to fill.
		* @param pieceID Piece type to fill the row with.
		* @return Reference to this context for chaining.
		*/
		ChessContext& BoardCmdFillRow(int rowIndex, ChessPieceID pieceID);

		/**
		* @brief Places a single piece at a specific board position.
		*
		* @param rowIndex Row index to place the piece.
		* @param columnIndex Column index to place the piece.
		* @param pieceID Piece type to place.
		* @param startRight If true, counts the column from the right side of the board.
		* @return Reference to this context for chaining.
		*/
		ChessContext& BoardCmdSetPiece(int rowIndex, int columnIndex, ChessPieceID pieceID, bool startRight = false);

		/**
		* @brief Places multiple pieces in a row.
		*
		* @param rowIndex Row index to place the pieces.
		* @param row Vector of piece IDs to place in order.
		* @param startRight If true, the row starts from the right side of the board.
		* @return Reference to this context for chaining.
		*/
		ChessContext& BoardCmdSetRow(int rowIndex, std::vector<ChessPieceID> row, bool startRight = false);

		/**
		* @brief Sets the size of the board.
		*
		* @param width Width of the board (number of columns).
		* @param height Height of the board (number of rows).
		* @return Reference to this context for chaining.
		*/
		ChessContext& SetBoardSize(int width, int height);

	private:
		/**
		* @brief Internal representation of a board setup command.
		*/
		struct BoardCommand {
			int columnIndex = 0;				/**< Column index for single piece placement */
			int rowIndex = 0;					/**< Row index */
			bool fill = false;					/**< True if the command fills an entire row */
			bool startRight = false;			/**< True if the placement starts from the right side */
			std::vector<ChessPieceID> pieces;	/**< Pieces to place */

			BoardCommand() = default;
		};

		int m_boardWidth = 0;
		int m_boardHeight = 0;
		std::vector<ChessPieceID> m_pieces;
		std::vector<BoardCommand> m_boardCmds;

		/**
		* @brief Adds a piece ID to the list if it does not already exist.
		*
		* @param pieceID Piece ID to add
		*/
		void AddUniquePiece(ChessPieceID pieceID);

		/**
		* @brief Applies all stored board commands to the given board.
		*
		* @param outBoard The board to apply commands to.
		*/
		void ApplyBoardCommands(ChessBoard& outBoard);

		void ApplyFillBoardRow(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const;
		void ApplyBoardRow(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const;
		void ApplySinglePiece(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const;
	
		/**
		* @brief Returns the mirrored row index for the black side.
		*
		* @param row Original row index.
		* @return int Mirrored row index.
		*/
		int MirrorRow(int row) const;

		/**
		* @brief Returns the mirrored column index for placements starting from the right.
		*
		* @param col Original column index.
		* @return int Mirrored column index.
		*/
		int MirrorColumn(int col) const;
	};

}