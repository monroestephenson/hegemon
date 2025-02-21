#pragma once

#include "db_connection.hpp"
#include <string>
#include <memory>
#include <pqxx/pqxx>

class PostgreSQLConnection : public DBConnection {
public:
    PostgreSQLConnection() = default;
    ~PostgreSQLConnection() = default;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    std::unique_ptr<pqxx::connection> conn;
    std::string currentDatabase;
    DatabaseConfig currentConfig;  // Store config for backup/restore operations
}; 