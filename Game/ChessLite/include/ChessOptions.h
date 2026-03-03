#pragma once

namespace OTN {
	class OTNObject;
}

class AppContext;
class ChessOptions {
public:
	bool showPossibleMoves = true;
	bool autoRetryGame = false;
	bool agentMoveDelay = true;
	float sfxVolume = 1.0f;

	ChessOptions() = default;
	~ChessOptions() = default;

	bool operator==(const ChessOptions&);
	bool operator!=(const ChessOptions&);

	void DecreaseSFXVolume(float amount = 0.1f);
	void IncreaseSFXVolume(float amount = 0.1f);

	bool SaveOptions();
	void LoadOptions(const OTN::OTNObject& optionsOTN);
};