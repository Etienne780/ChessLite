#include <CoreLib/Log.h>

#include "ChessContext.h"
#include "ChessBoard.h"

namespace CoreChess {

	ChessBoard ChessContext::GenerateBoard() {
		ChessBoard board{ m_boardWidth, m_boardHeight };
		ApplyBoardCommands(board);
		return board;
	}

	ChessContext& ChessContext::ReservePiece(size_t amount) {
		m_pieces.reserve(amount);
		return *this;
	}

	ChessContext& ChessContext::AddPiece(ChessPieceID pieceID) {
		AddUniquePiece(pieceID);
		return *this;
	}

	ChessContext& ChessContext::ClearBoardSetupCommands() {
		m_boardCmds.clear();
		return *this;
	}

	ChessContext& ChessContext::ReserveBoardCommands(size_t amount) {
		m_boardCmds.reserve(amount);
		return *this;
	}

	ChessContext& ChessContext::AddBoardCommand(const BoardCommand& cmd) {
		if (cmd.pieces.size() < 0)
			return *this;

		for (const auto& p : cmd.pieces) {
			if (p.IsInvalid())
				return *this;
			else
				AddUniquePiece(p);
		}

		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::AddBoardCommands(const std::vector<BoardCommand>& cmds) {
		for (const auto& cmd : cmds) {
			AddBoardCommand(cmd);
		}
		return *this;
	}

	ChessContext& ChessContext::BoardCmdFillRow(int rowIndex, ChessPieceID pieceID) {
		BoardCommand cmd;

		if (pieceID.IsInvalid())
			return *this;

		cmd.rowIndex = rowIndex;
		cmd.fill = true;
		cmd.pieces.push_back(pieceID);

		AddUniquePiece(pieceID);
		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::BoardCmdSetPiece(int rowIndex, int columnIndex, ChessPieceID pieceID, bool startRight) {
		BoardCommand cmd;

		if (pieceID.IsInvalid())
			return *this;

		cmd.rowIndex = rowIndex;
		cmd.columnIndex = columnIndex;
		cmd.startRight = startRight;
		cmd.pieces.push_back(pieceID);

		AddUniquePiece(pieceID);
		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::BoardCmdSetRow(int rowIndex, const std::vector<ChessPieceID>& row, bool startRight) {
		BoardCommand cmd;

		for (const auto& p : row) {
			if (p.IsInvalid())
				return *this;
			else
				AddUniquePiece(p);
		}

		cmd.rowIndex = rowIndex;
		cmd.startRight = startRight;
		cmd.pieces = row;

		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::SetBoardSize(int width, int height) {
		m_boardWidth = width;
		m_boardHeight = height;
		return *this;
	}

	int ChessContext::GetBoardWidth() const {
		return m_boardWidth;
	}

	int ChessContext::GetBoardHeight() const {
		return m_boardHeight;
	}

	const std::vector<ChessPieceID>& ChessContext::GetPieces() const {
		return m_pieces;
	}

	const std::vector<BoardCommand>& ChessContext::GetBoardCommands() const {
		return m_boardCmds;
	}

	void ChessContext::AddUniquePiece(ChessPieceID pieceID) {
		for (auto& p : m_pieces) {
			if (p == pieceID)
				return;
		}
		m_pieces.push_back(pieceID);
	}

	void ChessContext::ApplyBoardCommands(ChessBoard& outBoard) {
		auto applyCmd = [&](bool isWhite) -> void {
			for (const auto& cmd : m_boardCmds) {
				if (cmd.pieces.empty())
					continue;

				if (cmd.rowIndex >= m_boardHeight || cmd.rowIndex < 0)
					continue;

				if (cmd.fill) {
					ApplyFillBoardRow(cmd, isWhite, outBoard);
					continue;
				}

				if (cmd.columnIndex >= m_boardWidth || cmd.columnIndex < 0)
					continue;

				if (cmd.pieces.size() > 1) {
					ApplyBoardRow(cmd, isWhite, outBoard);
				}
				else {
					ApplySinglePiece(cmd, isWhite, outBoard);
				}
			}
		};

		applyCmd(true);
		applyCmd(false);
	}

	void ChessContext::ApplyFillBoardRow(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const {
		for (int i = 0; i < m_boardWidth; i++) {
			int rowIndex = (isWhite) ? cmd.rowIndex : MirrorRow(cmd.rowIndex);
			ChessPieceID piece = cmd.pieces[0];
			FieldType type = (isWhite) ? FieldType::WHITE : FieldType::BLACK;

			outBoard.SetFieldAt(i, rowIndex, type, piece);
		}
	}

	void ChessContext::ApplyBoardRow(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const {
		for (size_t i = 0; i < cmd.pieces.size(); i++) {
			int rowIndex = (isWhite) ? cmd.rowIndex : MirrorRow(cmd.rowIndex);
			int columnIndex = (cmd.startRight) ? MirrorColumn(static_cast<int>(i)) : static_cast<int>(i);

			ChessPieceID piece = cmd.pieces[i];
			FieldType type = (isWhite) ? FieldType::WHITE : FieldType::BLACK;

			outBoard.SetFieldAt(columnIndex, rowIndex, type, piece);
		}
	}

	void ChessContext::ApplySinglePiece(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const {
		int rowIndex = (isWhite) ? cmd.rowIndex : MirrorRow(cmd.rowIndex);
		int columnIndex = (cmd.startRight) ? MirrorColumn(cmd.columnIndex) : cmd.columnIndex;

		ChessPieceID piece = cmd.pieces[0];
		FieldType type = (isWhite) ? FieldType::WHITE : FieldType::BLACK;

		outBoard.SetFieldAt(columnIndex, rowIndex, type, piece);
	}

	int ChessContext::MirrorRow(int row) const {
		return m_boardHeight - 1 - row;
	}

	int ChessContext::MirrorColumn(int col) const {
		return m_boardWidth - 1 - col;
	}


}