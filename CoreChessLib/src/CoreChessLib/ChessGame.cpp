#include <CoreLib/Log.h>

#include "ChessGame.h"
#include "ChessContext.h"
#include "ChessPiece.h"
#include "ChessPieceRegistry.h"

namespace CoreChess {

	ChessGame::ChessGame(const ChessContext& ctx)
		: m_gameContext(ctx), m_isContextSet(true) {
	}

	void ChessGame::StartGame() {
		if (!CheckContext("StartGame"))
			return;

		if (m_gameState == ChessGameState::PLAYING) {
			Log::Error("CoreChess::ChessGame::StartGame: Faild to StartGame, a game is already running!");
			return;
		}

		SetGameState(ChessGameState::PLAYING);

		InternalDeselectPiece();
		m_isWhiteTurn = true;
		m_board = m_gameContext.GenerateBoard();
		m_winCondition = m_gameContext.GetWinCondition();
	}

	void ChessGame::EndGame() {
		if (!CheckContext("EndGame"))
			return;

		SetGameState(ChessGameState::IDLE);
	}

	bool ChessGame::IsPieceSelected() const {
		return m_isPieceSelected;
	}

	bool ChessGame::IsWhiteTurn() const {
		return m_isWhiteTurn;
	}

	ChessWinResult ChessGame::IsGameEnd() {
		if (m_winCondition)
			return m_winCondition(*this);

		Log::Error("CoreChess::ChessGame::IsGameEnd: Faild to check if the game ended, win condition is nullptr!");
		return ChessWinResult::NONE;
	}

	void ChessGame::SelectPiece(const Vector2& from) {
		if (!CheckContext("SelectPiece"))
			return;

		if (!IsInBoardBounds(from))
			return;

		const ChessField field = m_board.GetFieldAt(from);
		if (!CheckIfPieceCanPlay(field.GetFieldType()))
			return;

		InternalSelectPiece(from);
	}

	void ChessGame::DeselectPiece() {
		InternalDeselectPiece();
	}

	bool ChessGame::MovePiece(const Vector2& to) {
		if (!CheckContext("MovePiece"))
			return false;

		if (!m_isPieceSelected)
			return false;

		if (!IsInBoardBounds(to))
			return false;

		const ChessPieceRegistry& reg = ChessPieceRegistry::GetInstance();
		const ChessField field = m_board.GetFieldAt(m_selectedPiecePos);
		const ChessPiece* piece = reg.GetChessPiece(field.GetPieceID());

		if (!piece) {
			Log::Error("CoreChess::ChessGame::MovePiece: Faild to get piece, selected piece was invalid (PieceID: {})!", field.GetPieceID());
			return false;
		}

		if (!piece->IsValidMove(m_board, m_selectedPiecePos, to))
			return false;

		m_board.MovePieceFromTo(m_selectedPiecePos, to);

		m_isWhiteTurn = !m_isWhiteTurn;
		InternalDeselectPiece();
		return true;
	}

	size_t ChessGame::ConverToBoardIndex(const Vector2& pos) const {
		return m_board.PosToIndex(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	Vector2 ChessGame::ConverToBoardPosition(size_t index) const {
		int x = static_cast<int>(index % m_board.GetWidth());
		int y = static_cast<int>(index / m_board.GetWidth());
		return Vector2{ static_cast<float>(x), static_cast<float>(y) };
	}

	const ChessBoard& ChessGame::GetBoard() const {
		return m_board;
	}

	const Vector2& ChessGame::GetSelectedPiecePos() const {
		return m_selectedPiecePos;
	}

	size_t ChessGame::GetSelectedPieceIndex() const {
		return ConverToBoardIndex(GetSelectedPiecePos());
	}

	std::vector<Vector2> ChessGame::GetPossibleMovePosOf(size_t index) const {
		if (!IsInBoardBounds(index))
			return {};

		const auto& reg = ChessPieceRegistry::GetInstance();
		ChessField field = m_board.GetFieldAt(index);

		const ChessPiece* piece = reg.GetChessPiece(field.GetPieceID());
		if (!piece)
			return {};

		const Vector2 fromPos = ConverToBoardPosition(index);
		std::vector<Vector2> result;

		for (size_t i = 0; i < m_board.GetNumberOfFields(); i++) {
			Vector2 toPos = ConverToBoardPosition(i);
			if (piece->IsValidMove(m_board, fromPos, toPos)) {
				result.push_back(toPos);
			}
		}

		return result;
	}

	std::vector<size_t> ChessGame::GetPossibleMoveIndicesOf(size_t index) const {
		if (!IsInBoardBounds(index))
			return {};

		const auto& reg = ChessPieceRegistry::GetInstance();
		ChessField field = m_board.GetFieldAt(index);

		const ChessPiece* piece = reg.GetChessPiece(field.GetPieceID());
		if (!piece)
			return {};

		const Vector2 fromPos = ConverToBoardPosition(index);
		std::vector<size_t> result;

		for (size_t i = 0; i < m_board.GetNumberOfFields(); i++) {
			Vector2 toPos = ConverToBoardPosition(i);
			if (piece->IsValidMove(m_board, fromPos, toPos)) {
				result.push_back(i);
			}
		}

		return result;
	}

	void ChessGame::SetGameContext(const ChessContext& ctx) {
		if (m_gameState == ChessGameState::PLAYING) {
			Log::Error("CoreChess::ChessGame::SetGameContext: Failed to set context, game is running!");
			return;
		}

		m_isContextSet = true;
		m_gameContext = ctx;
	}

	void ChessGame::InternalSelectPiece(const Vector2& piecePos) {
		m_isPieceSelected = true;
		m_selectedPiecePos = piecePos;
	}

	void ChessGame::InternalDeselectPiece() {
		m_isPieceSelected = false;
		m_selectedPiecePos = Vector2::zero;
	}

	void ChessGame::SetGameState(ChessGameState state) {
		m_gameState = state;
	}

	bool ChessGame::CheckIfPieceCanPlay(FieldType type) const {
		if (type == FieldType::NONE)
			return false;
		
		if (m_isWhiteTurn)
			return type == FieldType::WHITE;

		return type == FieldType::BLACK;
	}

	bool ChessGame::CheckContext(const std::string& funcName) const {
		if (!m_isContextSet) {
			Log::Error("CoreChess::ChessGame::{}: Faild to {}, Chess context was not set!", funcName);
			return false;
		}
		return true;
	}

	bool ChessGame::IsInBoardBounds(const Vector2& pos) const {
		return pos.x < m_board.GetWidth() && pos.x >= 0 && 
			pos.y < m_board.GetHeight() && pos.y >= 0;
	}

	bool ChessGame::IsInBoardBounds(size_t index) const {
		size_t count = m_board.GetNumberOfFields();
		return index < count && index >= 0;
	}

}