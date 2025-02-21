#pragma once

#include "config.hpp"
#include "db_connection.hpp"
#include "compression.hpp"
#include <memory>
#include <string>

class BackupManager {
public:
    explicit BackupManager(const dbbackup::Config& cfg);
    ~BackupManager();

    bool backup(const std::string& backupType);
    bool restore(const std::string& backupPath);

protected:
    virtual std::unique_ptr<IDBConnection> createConnection();

private:
    dbbackup::Config m_config;
}; 