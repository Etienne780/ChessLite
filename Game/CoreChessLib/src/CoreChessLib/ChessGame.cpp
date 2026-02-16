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
		m_winResult = ChessWinResult::NONE;
		m_whiteMaterialValue = 0;
		m_blackMaterialValue = 0;
	}

	void ChessGame::EndGame() {
		if (!CheckContext("EndGame"))
			return;

		SetGameState(ChessGameState::IDLE);
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

	bool ChessGame::MovePiece(float toX, float toY) {
		return MovePiece(Vector2{ toX, toY });
	}

	bool ChessGame::MovePiece(const Vector2& to) {
		if (!CheckContext("MovePiece"))
			return false;

		if (m_winResult != ChessWinResult::NONE)
			return false;

		if (!m_isPieceSelected)
			return false;

		if (!IsInBoardBounds(to))
			return false;

		const ChessPieceRegistry& reg = ChessPieceRegistry::GetInstance();
		const ChessField fromField = m_board.GetFieldAt(m_selectedPiecePos);
		const ChessPiece* piece = reg.GetChessPiece(fromField.GetPieceID());

		if (!piece) {
			Log::Error("CoreChess::ChessGame::MovePiece: Faild to get piece, selected piece was invalid (PieceID: {})!", fromField.GetPieceID());
			return false;
		}

		if (!piece->IsValidMove(m_board, m_selectedPiecePos, to))
			return false;

		const ChessField toField = m_board.GetFieldAt(to);
		if (!toField.IsPieceNone()) {
			// capture
			PieceCaptured(toField.GetFieldType(), toField.GetPieceID());
		}

		m_board.MovePieceFromTo(m_selectedPiecePos, to);

		if (m_winCondition) {
			m_winResult = m_winCondition(*this);
		}
		else {
			Log::Error("CoreChess::ChessGame::MovePiece: Faild to check if the game ended, win condition is nullptr!");
		}

		m_isWhiteTurn = !m_isWhiteTurn;
		InternalDeselectPiece();
		return true;
	}

	size_t ChessGame::ConverToBoardIndex(const Vector2& pos) const {
		return m_board.PosToIndex(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	Vector2 ChessGame::ConverToBoardPosition(size_t index) const {
		return m_board.IndexToPos(index);
	}

	bool ChessGame::HasAnyLegalMove(FieldType playerColor) const {
		if (playerColor == FieldType::NONE)
			return true;

		const auto& reg = ChessPieceRegistry::GetInstance();

		for (size_t i = 0; i < m_board.GetNumberOfFields(); i++) {
			const ChessField& field = m_board.GetFieldAt(i);

			if (field.GetFieldType() != playerColor)
				continue;

			const ChessPiece* piece = reg.GetChessPiece(field.GetPieceID());
			if (!piece)
				continue;

			Vector2 from = ConverToBoardPosition(i);

			for (size_t j = 0; j < m_board.GetNumberOfFields(); j++) {
				Vector2 to = m_board.IndexToPos(j);
				const ChessField& otherField = m_board.GetFieldAt(j);

				if (!piece->IsValidMove(m_board, from, to))
					continue;

				m_board.MovePieceFromTo(from, to);

				bool legal = !IsAnyCriticalPieceAttacked(m_board, playerColor);
				m_board.SetFieldAt(from, field.GetFieldType(), field.GetPieceID());
				m_board.SetFieldAt(to, otherField.GetFieldType(), otherField.GetPieceID());

				if (legal)
					return true;
			}
		}

		return false;
	}

	bool ChessGame::IsPieceSelected() const {
		return m_isPieceSelected;
	}

	bool ChessGame::IsWhiteTurn() const {
		return m_isWhiteTurn;
	}

	bool ChessGame::IsGameEnd(ChessWinResult* outResult) const {
		if (outResult)
			*outResult = m_winResult;
		return m_winResult != ChessWinResult::NONE;
	}

	const ChessBoard& ChessGame::GetBoard() const {
		return m_board;
	}

	const ChessContext& ChessGame::GetContext() const {
		return m_gameContext;
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

	int ChessGame::GetWhiteMaterialValue() const {
		return m_whiteMaterialValue;
	}

	int ChessGame::GetBlackMaterialValue() const {
		return m_blackMaterialValue;
	}

	void ChessGame::SetGameContext(const ChessContext& ctx) {
		if (m_gameState == ChessGameState::PLAYING) {
			Log::Error("CoreChess::ChessGame::SetGameContext: Failed to set context, game is running!");
			return;
		}

		m_isContextSet = true;
		m_gameContext = ctx;
		
		auto& reg = ChessWinConditionRegistry::GetInstance();
		auto id = m_gameContext.GetWinConditionID();
		m_winCondition = reg.GetWinCondition(id);

		if (!m_winCondition) {
			Log::Warn("CoreChess::ChessGame::SetGameContext: Win condition of this function is nullptr!");
		}
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

	void ChessGame::PieceCaptured(FieldType capturedColor, ChessPieceID pieceID) {
		int* materialValue = nullptr;

		if (capturedColor == FieldType::WHITE)
			materialValue = &m_blackMaterialValue;
		else if (capturedColor == FieldType::BLACK)
			materialValue = &m_whiteMaterialValue;

		if (!materialValue)
			return;

		const auto& reg = ChessPieceRegistry::GetInstance();
		const ChessPiece* piece = reg.GetChessPiece(pieceID);

		if (!piece)
			return;

		*materialValue += piece->GetMaterialValue();
	}

	bool ChessGame::IsAnyCriticalPieceAttacked(const ChessBoard& board, FieldType color) const {
		FieldType enemyColor = (color == FieldType::WHITE)
			? FieldType::BLACK
			: FieldType::WHITE;

		const auto& reg = ChessPieceRegistry::GetInstance();

		for (size_t i = 0; i < board.GetNumberOfFields(); i++) {
			ChessField field = board.GetFieldAt(i);

			if (field.GetFieldType() != color)
				continue;

			const ChessPiece* piece = reg.GetChessPiece(field.GetPieceID());

			if (!piece || !piece->IsCriticalTarget())
				continue;

			Vector2 pos = ConverToBoardPosition(i);

			if (IsFieldAttacked(board, pos, enemyColor))
				return true;
		}

		return false;
	}

	bool ChessGame::IsFieldAttacked(const ChessBoard& board, const Vector2& pos, FieldType enemyColor) const {
		const auto& reg = ChessPieceRegistry::GetInstance();
		
		for (size_t i = 0; i < board.GetNumberOfFields(); i++) {
			ChessField field = board.GetFieldAt(i);
			
			if (field.GetFieldType() != enemyColor)
				continue;

			const ChessPiece* piece = reg.GetChessPiece(field.GetPieceID());

			if (!piece)
				continue;

			Vector2 enemyPos = ConverToBoardPosition(i);

			if (piece->IsValidMove(board, enemyPos, pos))
				return true;
		}

		return false;
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