#pragma once

#include "db_connection.hpp"
#include <string>

class MySQLConnection : public DBConnection {
public:
    MySQLConnection() = default;
    ~MySQLConnection() = default;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    std::string currentDatabase;
}; 