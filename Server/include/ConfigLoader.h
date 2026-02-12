#pragma once
#include "OTNFile.h"
#include "ServerLogic/SQLServerLogic.h"

DBConfig LoadDBConfigRelative(const OTN::OTNFilePath& relativePath);
DBConfig LoadDBConfig(const OTN::OTNFilePath& path);