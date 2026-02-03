#pragma once

namespace CoreChess::Internal {

	class Field {

	};

	class ChessBoard {
	public:
		ChessBoard(int x, int y);

	private:
		int m_boardSizeX = 0;
		int m_boardSizeY = 0;
	};

}