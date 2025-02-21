#pragma once

#include "../db_connection.hpp"
#include <mysql/mysql.h>
#include <string>

class MySQLConnection : public IDBConnection {
public:
    MySQLConnection() noexcept;
    ~MySQLConnection() noexcept override;

    bool connect(const dbbackup::DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    dbbackup::DatabaseConfig currentConfig;  // Store config for backup/restore operations
    MYSQL* mysql = nullptr;  // MySQL connection handle
    std::string currentDatabase;  // Current database name
}; 