#pragma once

#include "../db_connection.hpp"
#include <string>
#include <sqlite3.h>

class SQLiteConnection : public IDBConnection {
public:
    SQLiteConnection() noexcept;
    ~SQLiteConnection() noexcept override;

    bool connect(const dbbackup::DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    dbbackup::DatabaseConfig currentConfig;  // Store config for backup/restore operations
    sqlite3* db = nullptr;  // SQLite connection handle
    std::string currentDatabase;  // Current database path
}; 