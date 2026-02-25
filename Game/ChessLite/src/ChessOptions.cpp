#include <CoreLib/OTNFile.h>
#include <CoreLib/Log.h>

#include "ChessOptions.h"
#include "FilePaths.h"

bool ChessOptions::operator==(const ChessOptions& other) {
	return this->showPossibleMoves == other.showPossibleMoves &&
		this->autoRetryGame == other.autoRetryGame &&
		this->sfxVolume == other.sfxVolume;
}

bool ChessOptions::operator!=(const ChessOptions& other) {
	return !(*this == other);
}

void ChessOptions::DecreaseSFXVolume(float amount) {
	sfxVolume -= amount;
	if (sfxVolume < 0.0f)
		sfxVolume = 0.0f;
	sfxVolume = std::round(sfxVolume * 10.0f) / 10.0f;
}

void ChessOptions::IncreaseSFXVolume(float amount) {
	sfxVolume += amount;
	if (sfxVolume > 1.0f)
		sfxVolume = 1.0f;
	sfxVolume = std::round(sfxVolume * 10.0f) / 10.0f;
}

bool ChessOptions::SaveOptions() {
	using namespace OTN;

	OTNObject optionsObj{ "Options" };
	optionsObj.SetNames("showPossibleMoves", "autoRetryGame", "sfxVolume");
	optionsObj.SetTypes("bool", "bool", "float");
	optionsObj.AddDataRow(showPossibleMoves, autoRetryGame, sfxVolume);

	OTNWriter writer;
	writer.AppendObject(optionsObj);
	bool result = writer.Save(FilePaths::GetDataPath() / FilePaths::optionsFileName);
	
	if (!result)
		Log::Error("Failed to save file {}: {}", FilePaths::optionsFileName, writer.GetError());

	return result;
}