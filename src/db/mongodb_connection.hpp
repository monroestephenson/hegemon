#pragma once

#include "../db_connection.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <memory>

class MongoDBConnection : public IDBConnection {
public:
    MongoDBConnection();
    ~MongoDBConnection() override;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
    std::unique_ptr<mongocxx::instance> instance;
    std::unique_ptr<mongocxx::client> client;
    bool connected;
    std::string currentDbName;
}; 