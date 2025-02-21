#pragma once

#include "config.hpp"
#include "db_connection.hpp"
#include <memory>
#include <string>

class BackupManager {
public:
    explicit BackupManager(const Config& cfg);
    ~BackupManager();

    bool backup(const std::string& backupType);
    bool restore(const std::string& backupPath);

protected:
    virtual std::unique_ptr<IDBConnection> createConnection();

private:
    Config m_config;
}; 