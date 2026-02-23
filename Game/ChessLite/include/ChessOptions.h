#pragma once

class ChessOptions {
public:
	bool showPossibleMoves = true;

	ChessOptions() = default;
	~ChessOptions() = default;

	bool operator==(const ChessOptions&);
	bool operator!=(const ChessOptions&);

	bool SaveOptions();
};