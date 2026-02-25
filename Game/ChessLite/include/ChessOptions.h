#pragma once

class ChessOptions {
public:
	bool showPossibleMoves = true;
	bool autoRetryGame = false;

	ChessOptions() = default;
	~ChessOptions() = default;

	bool operator==(const ChessOptions&);
	bool operator!=(const ChessOptions&);

	bool SaveOptions();
};