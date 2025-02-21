#pragma once

#include "../db_connection.hpp"
#include <libpq-fe.h>
#include <memory>

class PostgreSQLConnection : public IDBConnection {
public:
    PostgreSQLConnection();
    ~PostgreSQLConnection() override;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    PGconn* conn;
    bool connected;
    std::string currentDbName;
}; 