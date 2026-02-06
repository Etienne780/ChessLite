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

		bool IsPieceSelected() const;
		bool IsWhiteTurn() const;
		ChessWinResult IsGameEnd();

		void SelectPiece(const Vector2& from);
		void DeselectPiece();
		bool MovePiece(const Vector2& to);

		size_t ConverToBoardIndex(const Vector2& pos) const;
		Vector2 ConverToBoardPosition(size_t index) const;

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
		ChessWinConditionFunc m_winCondition;

		ChessBoard m_board;
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

		bool IsInBoardBounds(const Vector2& pos) const;
		bool IsInBoardBounds(size_t index) const;
	};

}