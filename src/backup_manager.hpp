#pragma once

#include "config.hpp"
#include "db_connection.hpp"
#include <string>
#include <memory>

/// Orchestrates backup operations: full, incremental, or differential.
class BackupManager {
public:
    BackupManager(const Config& cfg);
    virtual ~BackupManager();

    /// Perform the backup. Type can be "full", "incremental", "differential"
    bool backup(const std::string& backupType = "full");

protected:
    /// Creates a database connection. Can be overridden in tests.
    virtual std::unique_ptr<IDBConnection> createConnection();

private:
    Config m_config;
};
