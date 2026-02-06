#pragma once
#include <CoreLib/Math/Vector2.h>

#include "CoreChessLib/ChessTypes.h"
#include "CoreChessLib/ChessField.h"

namespace CoreChess {

	/**
	* @brief Represents the current state of a chess board.
	*
	* Stores the board dimensions and all fields including
	* piece ownership and piece identifiers.
	*
	* This class does not enforce game rules or turn logic.
	*/
	class ChessBoard {
	public:
		ChessBoard() = default;
		ChessBoard(int w, int h);
		~ChessBoard() = default;

		/**
		* @brief Moves a piece from one position to another.
		*
		* Overwrites the target field and clears the source field.
		* No move validation is performed.
		*
		* @param from Source position.
		* @param to   Target position.
		*/
		void MovePieceFromTo(const Vector2& from, const Vector2& to);
		/**
		* @brief Moves a piece from one position to another.
		*
		* Overwrites the target field and clears the source field.
		* No move validation is performed.
		*
		* @param fromX Source x coordinate.
		* @param fromY Source y coordinate.
		* @param toX   Target x coordinate.
		* @param toY   Target y coordinate.
		*/
		void MovePieceFromTo(int fromX, int fromY, int toX, int toY);
		/**
		* @brief Moves a piece from one linear index to another.
		*
		* Overwrites the target field and clears the source field.
		* No move validation is performed.
		*
		* @param fromIndex Source field index.
		* @param toIndex   Target field index.
		*/
		void MovePieceFromTo(size_t fromIndex, size_t toIndex);
		
		/**
		* @brief Sets the field content at a given position.
		*
		* @param at   Board position.
		* @param type Field owner (e.g. white, black, none).
		* @param id   Piece identifier. (default is invalid).
		*/
		void SetFieldAt(const Vector2& at, FieldType type, ChessPieceID id = ChessPieceID(CORE_CHESS_INVALID_ID));
		/**
		* @brief Sets the field content at a given position.
		*
		* @param atX  X coordinate on the board.
		* @param atY  Y coordinate on the board.
		* @param type Field owner.
		* @param id   Piece identifier. (default is invalid).
		*/
		void SetFieldAt(int atX, int atY, FieldType type, ChessPieceID id = ChessPieceID(CORE_CHESS_INVALID_ID));
		/**
		* @brief Sets the field content at a given linear index.
		*
		* @param index Linear index of the field.
		* @param type  Field owner.
		* @param id    Piece identifier (default is invalid).
		*/
		void SetFieldAt(size_t index, FieldType type, ChessPieceID id = ChessPieceID(CORE_CHESS_INVALID_ID));

		/**
		* @brief Checks whether a piece exists at the given position.
		*
		* @return True if the field contains a piece.
		*/
		bool HasPieceAt(const Vector2& pos) const;
		/**
		* @brief Checks whether a piece exists at the given position.
		*
		* @param x X coordinate on the board.
		* @param y Y coordinate on the board.
		*
		* @return True if the field contains a piece.
		*/
		bool HasPieceAt(int x, int y) const;
		/**
		* @brief Checks whether a piece exists at the given linear index.
		*
		* @param index Linear index of the field.
		* @return True if a piece exists at the given index.
		*/
		bool HasPieceAt(size_t index) const;

		/**
		* @brief Converts board coordinates to a linear array index.
		*
		* @note Caller must ensure the coordinates are within bounds.
		*/
		size_t PosToIndex(int x, int y) const;

		/**
		* @brief Checks whether the field at the given position contains a piece
		*        of the specified field type.
		*
		* @param pos   Board position to check.
		* @param type  Expected field type (e.g. WHITE, BLACK).
		*
		* @return True if the field contains a piece of the given type.
		*/
		bool IsPieceAtEqual(const Vector2& pos, FieldType type) const;
		/**
		* @brief Checks whether the field at the given coordinates contains a piece
		*        of the specified field type.
		*
		* @param x     X coordinate on the board.
		* @param y     Y coordinate on the board.
		* @param type  Expected field type.
		*
		* @return True if the field contains a piece of the given type.
		*/
		bool IsPieceAtEqual(int x, int y, FieldType type) const;
		/**
		* @brief Checks whether the field at the given linear index contains a piece
		*        of the specified field type.
		*
		* @param index Linear index of the field.
		* @param type  Expected field type.
		* @return True if the field contains a piece of the given type.
		*/
		bool IsPieceAtEqual(size_t index, FieldType type) const;

		/**
		* @brief Returns the field data at the given position.
		*
		* @param pos Board position.
		*
		* @return The ChessField stored at the given position.
		*         If the position is out of bounds, an empty field is returned.
		*/
		ChessField GetFieldAt(const Vector2& pos) const;
		/**
		* @brief Returns the field data at the given coordinates.
		*
		* @param x X coordinate on the board.
		* @param y Y coordinate on the board.
		*
		* @return The ChessField stored at the given position.
		*         If the position is out of bounds, an empty field is returned.
		*/
		ChessField GetFieldAt(int x, int y) const;
		/**
		* @brief Returns the field data at the given linear index.
		*
		* @param index Linear index of the field.
		* @return ChessField at the given index.
		*/
		ChessField GetFieldAt(size_t index) const;

		/**
		* @brief Returns all fields on the board.
		*
		* The returned reference gives access to the underlying list of fields
		* in row-major order.
		*
		* @return Const reference to the vector of ChessField objects.
		*/
		const std::vector<ChessField>& GetFields() const;

		/**
		* @brief Gets the width of the board.
		*
		* @return Number of columns on the board.
		*/
		int GetWidth() const;

		/**
		* @brief Gets the height of the board.
		*
		* @return Number of rows on the board.
		*/
		int GetHeight() const;

		/**
		* @brief Returns the total number of fields on the board.
		*
		* This is equivalent to width * height.
		*
		* @return Total number of fields as a size_t.
		*/
		size_t GetNumberOfFields() const;


	private:
		int m_boardWidth = 0;
		int m_boardHeight = 0;
		std::vector<ChessField> m_board;

		/**
		* @brief Generates a blank board with the given dimensions.
		*
		* All fields are initialized to FieldType::NONE and an invalid piece ID.
		*
		* @param width  Number of columns.
		* @param height Number of rows.
		* @return Vector of ChessField objects representing the empty board.
		*/
		static std::vector<ChessField> GenerateBoard(int width, int height);
	};

}