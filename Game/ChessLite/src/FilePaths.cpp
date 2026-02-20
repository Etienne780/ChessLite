#include "FilePaths.h"
#include "App.h"

namespace FilePaths {
	static SystemFilePath s_exePath;
	static SystemFilePath s_assetsPath;
	static SystemFilePath s_dataPath;

	const SystemFilePath& GetExePath() {
		return s_exePath;
	}

	const SystemFilePath& GetAssetsPath() {
		return s_assetsPath;
	}

	const SystemFilePath& GetDataPath() {
		return s_dataPath;
	}

	bool InitPaths() {
		auto* app = App::GetInstance();
		if (!app)
			return false;

		s_exePath = app->GetBasePath();

		s_assetsPath = s_exePath / "assets";
		s_dataPath = s_exePath / "data";

		return true;
	}
}