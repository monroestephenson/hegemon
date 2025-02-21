#pragma once

#include "db_connection.hpp"
#include <string>

class SQLiteConnection : public DBConnection {
public:
    SQLiteConnection() = default;
    ~SQLiteConnection() = default;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    std::string currentDatabase;
}; 