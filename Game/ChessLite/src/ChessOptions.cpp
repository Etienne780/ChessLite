#include <CoreLib/OTNFile.h>
#include <CoreLib/Log.h>

#include "ChessOptions.h"
#include "FilePaths.h"

bool ChessOptions::operator==(const ChessOptions& other) {
	return this->showPossibleMoves == other.showPossibleMoves &&
		this->autoRetryGame == other.autoRetryGame;
}

bool ChessOptions::operator!=(const ChessOptions& other) {
	return !(*this == other);
}

bool ChessOptions::SaveOptions() {
	using namespace OTN;

	OTNObject optionsObj{ "Options" };
	optionsObj.SetNames("showPossibleMoves", "autoRetryGame");
	optionsObj.SetTypes("bool", "bool");
	optionsObj.AddDataRow(showPossibleMoves, autoRetryGame);

	OTNWriter writer;
	writer.AppendObject(optionsObj);
	bool result = writer.Save(FilePaths::GetDataPath() / FilePaths::optionsFileName);
	
	if (!result)
		Log::Error("Failed to save file {}: {}", FilePaths::optionsFileName, writer.GetError());

	return result;
}