#pragma once

#include "db_connection.hpp"
#include <string>
#include <mysql/mysql.h>

class MySQLConnection : public DBConnection {
public:
    MySQLConnection();
    ~MySQLConnection();

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    MYSQL* mysql;
    std::string currentDatabase;
    DatabaseConfig currentConfig;  // Store config for backup/restore operations
}; 