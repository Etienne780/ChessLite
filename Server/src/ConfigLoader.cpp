#include <whereami/whereami.h>

#include "ConfigLoader.h"

DBConfig LoadDBConfigRelative(const OTN::OTNFilePath& relativePath) {
    int length = wai_getExecutablePath(nullptr, 0, nullptr);
    if (length <= 0)
        throw std::runtime_error("LoadDBConfigRelative: Cannot determine executable path");

    std::vector<char> exePath(length + 1);
    int result = wai_getExecutablePath(exePath.data(), static_cast<int>(exePath.size()), nullptr);
    if (result <= 0)
        throw std::runtime_error("LoadDBConfigRelative: Failed to retrieve executable path");

    std::filesystem::path exeDir(exePath.data());
    exeDir = exeDir.parent_path();
    std::filesystem::path configPath = exeDir / relativePath;

    if (!std::filesystem::exists(configPath))
        throw std::runtime_error("LoadDBConfigRelative: Config file does not exist: " + configPath.string());

    return LoadDBConfig(configPath.string());
}

DBConfig LoadDBConfig(const OTN::OTNFilePath& path) {
    DBConfig cfg;

    OTN::OTNReader reader;
    if (!reader.ReadFile(path)) {
        std::string errMsg = "LoadDBConfig: Failed to read config file: " + path.string() + "\n Reader error:" + reader.GetError();
        throw std::runtime_error(errMsg);
    }

    if (auto objOpt = reader.TryGetObject("database")) {
        if (auto hostOpt = objOpt->TryGetValue<std::string>(0, "host")) 
            cfg.host = *hostOpt;
        if (auto portOpt = objOpt->TryGetValue<int>(0, "port")) 
            cfg.port = static_cast<uint16_t>(*portOpt);
        if (auto userOpt = objOpt->TryGetValue<std::string>(0, "user")) 
            cfg.user = *userOpt;
        if (auto passOpt = objOpt->TryGetValue<std::string>(0, "password")) 
            cfg.password = *passOpt;
        if (auto schemaOpt = objOpt->TryGetValue<std::string>(0, "schema")) 
            cfg.schema = *schemaOpt;
    }
    else {
        throw std::runtime_error("LoadDBConfig: Database object not found in config file: " + path.string());
    }

    return cfg;
}