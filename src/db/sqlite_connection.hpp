#pragma once

#include "../db_connection.hpp"
#include <sqlite3.h>
#include <memory>

class SQLiteConnection : public IDBConnection {
public:
    SQLiteConnection();
    ~SQLiteConnection() override;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    sqlite3* db;
    bool connected;
    std::string dbPath;
}; 