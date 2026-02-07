#pragma once
#include <cstdint>
#include <CoreLib/Math/Vector2.h>

#include "ChessTypes.h"
#include "ChessContext.h"
#include "ChessBoard.h"

namespace CoreChess {

	class ChessGame {
	public:
		ChessGame() = default;
		ChessGame(const ChessContext& ctx);

		void StartGame();
		void EndGame();

		void SelectPiece(const Vector2& from);
		void DeselectPiece();
		bool MovePiece(const Vector2& to);

		size_t ConverToBoardIndex(const Vector2& pos) const;
		Vector2 ConverToBoardPosition(size_t index) const;

		bool HasAnyLegalMove(FieldType playerColor) const;

		bool IsPieceSelected() const;
		bool IsWhiteTurn() const;
		bool IsGameEnd(ChessWinResult* outResult = nullptr) const;

		const ChessBoard& GetBoard() const;
		const Vector2& GetSelectedPiecePos() const;
		size_t GetSelectedPieceIndex() const;
		std::vector<Vector2> GetPossibleMovePosOf(size_t index) const;
		std::vector<size_t> GetPossibleMoveIndicesOf(size_t index) const;
		int GetWhiteMaterialValue() const;
		int GetBlackMaterialValue() const;

		void SetGameContext(const ChessContext& ctx);

	private:
		bool m_isContextSet = false;
		ChessContext m_gameContext;
		ChessGameState m_gameState = ChessGameState::IDLE;
		ChessWinConditionFunc m_winCondition = nullptr;
		ChessWinResult m_winResult = ChessWinResult::NONE;

		mutable ChessBoard m_board;
		bool m_isWhiteTurn = true;
		bool m_isPieceSelected = false;
		Vector2 m_selectedPiecePos = Vector2::zero;

		int m_whiteMaterialValue = 0;
		int m_blackMaterialValue = 0;

		void InternalSelectPiece(const Vector2& piecePos);
		void InternalDeselectPiece();

		void SetGameState(ChessGameState state);

		/**
		* @brief Checks whether a piece can be played by the current player.
		* 
		* @param type FieldType of the piece on the board.
		* @return True if the piece belongs to the current player.
		*/
		bool CheckIfPieceCanPlay(FieldType type) const;
		bool CheckContext(const std::string& funcName) const;

		void PieceCaptured(FieldType capturedColor, ChessPieceID pieceID);

		/**
		* @brief Checks whether any critical piece of the given color is under attack.
		*
		* Iterates over all pieces of the specified color and evaluates whether
		* any piece marked as a critical target is attacked by the opponent.
		*
		* This function is typically used to validate move legality.
		*
		* @param board The board state to evaluate.
		* @param color The player color whose critical pieces are checked.
		*
		* @return True if at least one critical piece is attacked, false otherwise.
		*/
		bool IsAnyCriticalPieceAttacked(const ChessBoard& board,FieldType color) const;
		bool IsFieldAttacked(const ChessBoard& board, const Vector2& pos, FieldType enemyColor) const;
		bool IsInBoardBounds(const Vector2& pos) const;
		bool IsInBoardBounds(size_t index) const;
	};

}