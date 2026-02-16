#pragma once
#include <vector>
#include <unordered_map>

#include "ChessTypes.h"
#include "ChessWinConditionRegistry.h"

namespace CoreChess {

	class ChessBoard;

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

		/**
		* @brief Pre-allocates memory for a number of board setup commands.
		*
		* @param amount Number of board commands to reserve space for.
		* @return Reference to this context for chaining.
		*/
		ChessContext& ReserveBoardCommands(size_t amount);

		/**
		* @brief Adds a single board setup command to the context.
		*
		* Validates the pieces in the command before adding.
		*
		* @param cmd BoardCommand to add.
		* @return Reference to this context for chaining.
		*/
		ChessContext& AddBoardCommand(const BoardCommand& cmd);

		/**
		* @brief Adds multiple board setup commands to the context.
		*
		* Internally calls AddBoardCommand for each entry.
		*
		* @param cmds Vector of BoardCommands to add.
		* @return Reference to this context for chaining.
		*/
		ChessContext& AddBoardCommands(const std::vector<BoardCommand>& cmds);

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
		ChessContext& BoardCmdSetRow(int rowIndex, const std::vector<ChessPieceID>& row, bool startRight = false);

		/**
		* @brief Sets the size of the board.
		*
		* @param width Width of the board (number of columns).
		* @param height Height of the board (number of rows).
		* @return Reference to this context for chaining.
		*/
		ChessContext& SetBoardSize(int width, int height);
		
		/**
		* @brief Registers and assigns a win condition function.
		*
		* The given function is registered in the ChessWinConditionRegistry,
		* which generates a new unique ChessWinConditionID. The context then
		* stores and uses this generated ID internally.
		*
		* Optionally, the generated identifier can be returned via the outID
		* pointer. If outID is nullptr, the identifier is not exposed to the caller.
		*
		* @param func The win condition function to register and assign.
		* @param outID Optional pointer that receives the generated ChessWinConditionID.
		* @return Reference to this ChessContext instance for chaining.
		*/
		ChessContext& SetWinCondition(const ChessWinConditionFunc& func, ChessWinConditionID* outID = nullptr);

		/**
		* @brief Assigns an existing win condition by its identifier.
		*
		* The provided ChessWinConditionID must already be registered in
		* the ChessWinConditionRegistry. In debug builds, validity checks
		* are performed and warnings are logged if the ID is invalid or
		* not associated with a function.
		*
		* @param id The identifier of a previously registered win condition.
		* @return Reference to this ChessContext instance for chaining.
		*/
		ChessContext& SetWinCondition(ChessWinConditionID id);

		/**
		* @brief Returns the width of the board for this context.
		*
		* @return Number of columns.
		*/
		int GetBoardWidth() const;

		/**
		* @brief Returns the height of the board for this context.
		*
		* @return Number of rows.
		*/
		int GetBoardHeight() const;

		/**
		* @brief Returns all pieces added to this context.
		*
		* @return Vector of ChessPieceID currently registered.
		*/
		const std::vector<ChessPieceID>& GetPieces() const;

		/**
		* @brief Returns all board setup commands currently stored in the context.
		*
		* @return Vector of BoardCommand.
		*/
		const std::vector<BoardCommand>& GetBoardCommands() const;

		/**
		* @brief Returns the currently assigned win condition identifier.
		*
		* The returned ID can be used to query the associated win condition
		* function from the ChessWinConditionRegistry.
		*
		* @return The active ChessWinConditionID.
		*/
		ChessWinConditionID GetWinConditionID() const;

		/**
		* @brief Serializes the current configuration of the context into a Base64 string.
		*
		* All relevant configuration data (board size, win condition,
		* pieces, and board commands) are serialized into a binary format
		* and encoded as Base64.
		*
		* @return std::string Base64-encoded representation of the current configuration.
		*/
		std::string GetConfigString() const;

		/**
		* @brief Restores the context configuration from a Base64 string.
		*
		* Decodes the given Base64 string, deserializes the contained binary data,
		* and applies the configuration to the context. The function provides
		* strong exception safety: the internal state is only updated if the
		* entire deserialization process succeeds.
		*
		* @param config Base64-encoded configuration string.
		* @return true  If the configuration was successfully parsed and applied.
		* @return false If decoding, version validation, or deserialization failed.
		*/
		bool SetPerConfigString(const std::string& config);

	private:
		int m_boardWidth = 0;
		int m_boardHeight = 0;
		std::vector<ChessPieceID> m_pieces;
		std::vector<BoardCommand> m_boardCmds;
		ChessWinConditionID m_winConditionID;

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