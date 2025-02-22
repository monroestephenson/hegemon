#pragma once

#include "../db_connection.hpp"
#include <pqxx/pqxx>
#include <string>
#include <memory>

namespace dbbackup {

class PostgreSQLConnection : public IDBConnection {
public:
    PostgreSQLConnection() noexcept;
    ~PostgreSQLConnection() noexcept override;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    DatabaseConfig currentConfig;  // Store config for backup/restore operations
    std::unique_ptr<pqxx::connection> conn;  // PostgreSQL connection handle
    std::string currentDatabase;  // Current database name
};

} // namespace dbbackup 