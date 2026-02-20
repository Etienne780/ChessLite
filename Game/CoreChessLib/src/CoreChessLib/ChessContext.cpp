#include <CoreLib/Log.h>
#include <CoreLib/BinarySerializer.h>
#include <CoreLib/BinaryDeserializer.h>
#include <CoreLib/ConversionUtils.h>

#include "ChessContext.h"
#include "ChessBoard.h"

namespace CoreChess {

	constexpr uint32_t CONFIG_VERSION = 1;

	ChessContext::ChessContext(const std::string& config) {
		SetPerConfigString(config);
	}

	ChessBoard ChessContext::GenerateBoard() {
		ChessBoard board{ m_boardWidth, m_boardHeight };
		ApplyBoardCommands(board);
		return board;
	}

	ChessContext& ChessContext::ReservePieces(size_t amount) {
		m_pieces.reserve(amount);
		return *this;
	}

	ChessContext& ChessContext::AddPiece(ChessPieceID pieceID) {
		AddUniquePiece(pieceID);
		return *this;
	}

	ChessContext& ChessContext::ClearBoardSetupCommands() {
		SetDirty();
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

		size_t index = 0;
		for (const auto& p : cmd.pieces) {
			if (p.IsInvalid()) {
				Log::Error("CoreChess::ChessContext::AddBoardCommand: Piece id of index '{}' is invalid!", index);
				return *this;
			}
			else {
				AddUniquePiece(p);
			}
			index++;
		}

		SetDirty();
		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::AddBoardCommands(const std::vector<BoardCommand>& cmds) {
		SetDirty();
		for (const auto& cmd : cmds) {
			AddBoardCommand(cmd);
		}
		return *this;
	}

	ChessContext& ChessContext::BoardCmdFillRow(int rowIndex, ChessPieceID pieceID) {
		BoardCommand cmd;

		if (pieceID.IsInvalid()) {
			Log::Error("CoreChess::ChessContext::BoardCmdFillRow: piece id is invalid!");
			return *this;
		}

		cmd.rowIndex = rowIndex;
		cmd.fill = true;
		cmd.pieces.push_back(pieceID);

		AddUniquePiece(pieceID);
		SetDirty();
		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::BoardCmdSetPiece(int rowIndex, int columnIndex, ChessPieceID pieceID, bool startRight) {
		BoardCommand cmd;

		if (pieceID.IsInvalid()) {
			Log::Error("CoreChess::ChessContext::BoardCmdSetPiece: piece id is invalid!");
			return *this;
		}

		cmd.rowIndex = rowIndex;
		cmd.columnIndex = columnIndex;
		cmd.startRight = startRight;
		cmd.pieces.push_back(pieceID);

		AddUniquePiece(pieceID);
		SetDirty();
		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::BoardCmdSetRow(int rowIndex, const std::vector<ChessPieceID>& row, bool startRight) {
		BoardCommand cmd;

		size_t index = 0;
		for (const auto& p : row) {
			if (p.IsInvalid()) {
				Log::Error("CoreChess::ChessContext::AddBoardCommand: Piece id of index '{}' is invalid!", index);
				return *this;
			}
			else {
				AddUniquePiece(p);
			}
			index++;
		}

		cmd.rowIndex = rowIndex;
		cmd.startRight = startRight;
		cmd.pieces = row;

		SetDirty();
		m_boardCmds.push_back(cmd);
		return *this;
	}

	ChessContext& ChessContext::SetBoardSize(int width, int height) {
		SetDirty();
		m_boardWidth = width;
		m_boardHeight = height;
		return *this;
	}

	ChessContext& ChessContext::SetWinCondition(
		const ChessWinConditionFunc& func, 
		ChessWinConditionID* outID) 
	{
		auto& reg = ChessWinConditionRegistry::GetInstance();
		auto id = reg.AddWinCondition(func);
		if (outID)
			*outID = id;
		return SetWinCondition(id);
	}

	ChessContext& ChessContext::SetWinCondition(ChessWinConditionID id) {
#ifndef NDEBUG
		if (id.IsInvalid()) {
			Log::Warn("CoreChess::ChessContext::SetWinCondition: Given id was invalid");
		}

		auto& reg = ChessWinConditionRegistry::GetInstance();
		auto func = reg.GetWinCondition(id);
		if (!func) {
			Log::Warn("CoreChess::ChessContext::SetWinCondition: There is no win condition func asoziated with the id '{}'", id);
		}
#endif
		SetDirty();
		m_winConditionID = id;
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

	ChessWinConditionID ChessContext::GetWinConditionID() const {
		return m_winConditionID;
	}

	std::string ChessContext::GetConfigString() const {
		if (!m_configContextChanged) {
			return m_configString;
		}
		
		BinarySerializer bSer;

		bSer.AddFields(
			CONFIG_VERSION,
			m_boardWidth, 
			m_boardHeight,
			m_winConditionID,
			m_pieces
		);
		
		bSer.AddComplexField(m_boardCmds, 
		[](BinarySerializer& subSer, const CoreChess::BoardCommand& cmd) {
			uint8_t flags = 0;
			flags |= cmd.fill ? 1 : 0;
			flags |= cmd.startRight ? 2 : 0;
			
			subSer.AddFields(
				cmd.columnIndex,
				cmd.rowIndex,
				flags,
				cmd.pieces
			);
		});

		m_configContextChanged = false;
		m_configString = bSer.ToBase64();
		return m_configString;
	}

	bool ChessContext::SetPerConfigString(const std::string& config) {
		try {
			std::vector<uint8_t> buffer = ConversionUtils::FromBase64(config);
			BinaryDeserializer bDes{ buffer };

			uint32_t version = bDes.Read<uint32_t>();
			if (version != CONFIG_VERSION) {
				Log::Error("Unsupported config version '{}'", version);
				return false;
			}

			int boardWidth = bDes.Read<int>();
			int boardHeight = bDes.Read<int>();
			auto winConditionID = bDes.Read<ChessWinConditionID>();
			auto pieces = bDes.ReadVector<ChessPieceID>();

			auto boardCmds = bDes.ReadVector<CoreChess::BoardCommand>(
			[](BinaryDeserializer& subDes) {
				CoreChess::BoardCommand cmd{};

				cmd.columnIndex = subDes.Read<int>();
				cmd.rowIndex = subDes.Read<int>();

				uint8_t flags = subDes.Read<uint8_t>();
				cmd.fill = (flags & 1) != 0;
				cmd.startRight = (flags & 2) != 0;

				cmd.pieces = subDes.ReadVector<ChessPieceID>();

				return cmd;
			});

			if (!bDes.IsAtEnd()) {
				Log::Error("Config contains trailing data");
				return false;
			}

			m_boardWidth = boardWidth;
			m_boardHeight = boardHeight;
			m_winConditionID = winConditionID;
			m_pieces = std::move(pieces);
			m_boardCmds = std::move(boardCmds);
		}
		catch (const std::exception& e) {
			Log::Error("CoreChess::ChessContext::SetPerConfigString: Failed to set: {}", e.what());
			return false;
		}
		return true;
	}

	void ChessContext::AddUniquePiece(ChessPieceID pieceID) {
		for (auto& p : m_pieces) {
			if (p == pieceID)
				return;
		}
		SetDirty();
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
			int rowIndex = (isWhite) ? MirrorRow(cmd.rowIndex) : cmd.rowIndex;
			ChessPieceID piece = cmd.pieces[0];
			FieldType type = (isWhite) ? FieldType::WHITE : FieldType::BLACK;

			outBoard.SetFieldAt(i, rowIndex, type, piece);
		}
	}

	void ChessContext::ApplyBoardRow(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const {
		for (size_t i = 0; i < cmd.pieces.size(); i++) {
			int rowIndex = (isWhite) ? MirrorRow(cmd.rowIndex) : cmd.rowIndex;
			int columnIndex = (cmd.startRight) ? MirrorColumn(static_cast<int>(i)) : static_cast<int>(i);

			ChessPieceID piece = cmd.pieces[i];
			FieldType type = (isWhite) ? FieldType::WHITE : FieldType::BLACK;

			outBoard.SetFieldAt(columnIndex, rowIndex, type, piece);
		}
	}

	void ChessContext::ApplySinglePiece(const BoardCommand& cmd, bool isWhite, ChessBoard& outBoard) const {
		int rowIndex = (isWhite) ? MirrorRow(cmd.rowIndex) : cmd.rowIndex;
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

	void ChessContext::SetDirty() {
		m_configContextChanged = true;
	}

}