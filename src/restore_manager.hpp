#pragma once

#include "config.hpp"
#include <string>

class RestoreManager {
public:
    RestoreManager(const Config& cfg);
    ~RestoreManager();

    /// Restores the database from a given backup file path (compressed or uncompressed).
    bool restore(const std::string& backupFilePath, bool selectiveRestore = false);

private:
    Config m_config;
};
