#pragma once
#include <vector>
#include <unordered_map>

#include "ChessTypes.h"

namespace CoreChess {

	class ChessBoard;

	/*
	* @stores data for a chess game like what pieces there are.
	* 
	* what board, what the starting position...
	*/
	class ChessContext {
	public:
		ChessContext() = default;

		ChessBoard GenerateBoard();

		ChessContext& ReservePiece(size_t amount);
		ChessContext& AddPiece(ChessPieceID pieceID);

		ChessContext& ClearBoardSetupCommands();

		ChessContext& BoardCmdFillRow(int rowIndex, ChessPieceID pieceID);
		ChessContext& BoardCmdSetPiece(int rowIndex, int columnIndex, ChessPieceID pieceID, bool startRight = false);
		ChessContext& BoardCmdSetRow(int rowIndex, std::vector<ChessPieceID> row, bool startRight = false);

		ChessContext& SetBoardSize(int width, int height);

	private:
		struct BoardCommand {
			int columnIndex = 0;
			int rowIndex = 0;
			bool fill = false;// < fills a whole row with piece
			bool startRight = false;
			std::vector<ChessPieceID> pieces;

			BoardCommand() = default;
		};

		int m_boardWidth = 0;
		int m_boardHeight = 0;
		std::vector<ChessPieceID> m_pieces;
		std::vector<BoardCommand> m_boardCmds;

		void AddUniquePiece(ChessPieceID pieceID);
		void ApplyBoardCommands(ChessBoard& outBoard);
		void ApplyFillBoardRow(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const;
		void ApplyBoardRow(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const;
		void ApplySinglePiece(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const;
	};

}