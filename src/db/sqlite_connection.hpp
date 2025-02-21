#pragma once

#include "db_connection.hpp"
#include <string>
#include <sqlite3.h>

class SQLiteConnection : public DBConnection {
public:
    SQLiteConnection();
    ~SQLiteConnection();

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    sqlite3* db;
    std::string currentDatabase;
}; 