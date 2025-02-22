#pragma once

#include "../db_connection.hpp"
#include <pqxx/pqxx>
#include <string>
#include <memory>

class PostgreSQLConnection : public IDBConnection {
public:
    PostgreSQLConnection() noexcept;
    ~PostgreSQLConnection() noexcept override;

    bool connect(const dbbackup::DatabaseConfig& config) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    dbbackup::DatabaseConfig currentConfig;  // Store config for backup/restore operations
    std::unique_ptr<pqxx::connection> conn;  // PostgreSQL connection handle
    std::string currentDatabase;  // Current database name
}; 