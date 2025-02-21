#pragma once

#include "config.hpp"
#include <string>

/// Orchestrates backup operations: full, incremental, or differential.
class BackupManager {
public:
    BackupManager(const Config& cfg);
    ~BackupManager();

    /// Perform the backup. Type can be "full", "incremental", "differential"
    bool backup(const std::string& backupType = "full");

private:
    Config m_config;
};
