#pragma once

#include "config.hpp"
#include <memory>
#include <string>

class IDBConnection {
public:
    virtual ~IDBConnection() = default;
    virtual bool connect(const DatabaseConfig& dbConfig) = 0;
    virtual bool disconnect() = 0;

    // Example methods for backup/restore:
    virtual bool createBackup(const std::string& backupPath) = 0;
    virtual bool restoreBackup(const std::string& backupPath) = 0;
};

/// Factory function to create a database connection object depending on dbConfig.type
std::unique_ptr<IDBConnection> createDBConnection(const DatabaseConfig& dbConfig);
