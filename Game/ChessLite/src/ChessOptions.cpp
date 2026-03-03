#include <CoreLib/OTNFile.h>
#include <CoreLib/Log.h>
#include <SDLCoreLib/SDLCore.h>

#include "ChessOptions.h"
#include "Type.h"
#include "FilePaths.h"

bool ChessOptions::operator==(const ChessOptions& other) {
	return this->showPossibleMoves == other.showPossibleMoves &&
		this->autoRetryGame == other.autoRetryGame &&
		this->agentMoveDelay == other.agentMoveDelay &&
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
	optionsObj.SetNames("showPossibleMoves", "autoRetryGame", "agentMoveDelay", "sfxVolume");
	optionsObj.SetTypes("bool", "bool", "bool", "float");
	optionsObj.AddDataRow(showPossibleMoves, autoRetryGame, agentMoveDelay, sfxVolume);

	OTNWriter writer;
	writer.AppendObject(optionsObj);
	bool result = writer.Save(FilePaths::GetDataPath() / FilePaths::optionsFileName);
	
	if (!result)
		Log::Error("Failed to save file {}: {}", FilePaths::optionsFileName, writer.GetError());

	return result;
}

void ChessOptions::LoadOptions(const OTN::OTNObject& optionsOTN) {
	if (auto obj = optionsOTN.TryGetValue<bool>(0, "showPossibleMoves")) {
		showPossibleMoves = *obj;
	}

	if (auto obj = optionsOTN.TryGetValue<bool>(0, "autoRetryGame")) {
		autoRetryGame = *obj;
	}

	if (auto obj = optionsOTN.TryGetValue<bool>(0, "agentMoveDelay")) {
		agentMoveDelay = *obj;
	}

	if (auto obj = optionsOTN.TryGetValue<float>(0, "sfxVolume")) {
		sfxVolume = *obj;
		SDLCore::SoundManager::SetTagVolume(SDLCore::SoundTags::SFX, *obj);
	}
}