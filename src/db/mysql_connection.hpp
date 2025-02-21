#pragma once

#include "../db_connection.hpp"
#include <mysql.h>
#include <memory>

class MySQLConnection : public IDBConnection {
public:
    MySQLConnection();
    ~MySQLConnection() override;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    MYSQL* mysql;
    bool connected;
    std::string currentDbName;
}; 