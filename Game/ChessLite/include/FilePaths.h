#pragma once
#include <CoreLib/OTNFile.h>
#include <CoreLib/File.h>

namespace FilePaths {

	/*
	* @brief Gets the path where the application (exe) is located
	* @return SystemFilePath reference
	*/
	const SystemFilePath& GetExePath();

	/*
	* @brief Gets the base path where the Assets are located
	* 
	* note: is an absolute path
	* 
	* @return SystemFilePath reference
	*/
	const SystemFilePath& GetAssetsPath();

	/*
	* @brief Gets the base path where the data is located
	*
	* note: is an absolute path
	*
	* @return SystemFilePath reference
	*/
	const SystemFilePath& GetDataPath();
	
	/*
	* @brief Initialises all Paths. Should only be called once
	* @return true on success
	*/
	bool InitPaths();

}