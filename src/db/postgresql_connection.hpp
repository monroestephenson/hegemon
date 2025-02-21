#pragma once

#include "db_connection.hpp"
#include <string>

class PostgreSQLConnection : public DBConnection {
public:
    PostgreSQLConnection() = default;
    ~PostgreSQLConnection() = default;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    std::string currentDatabase;
}; 