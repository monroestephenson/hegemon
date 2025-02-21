#pragma once

#include "config.hpp"
#include <memory>
#include <string>

/// Interface for database connections
class IDBConnection {
public:
    virtual ~IDBConnection() = default;
    virtual bool connect(const DatabaseConfig& dbConfig) = 0;
    virtual bool disconnect() = 0;

    /// Create a backup at the specified path
    virtual bool createBackup(const std::string& backupPath) = 0;
    virtual bool restoreBackup(const std::string& backupPath) = 0;
};

/// Concrete implementation of database connection
class DBConnection : public IDBConnection {
public:
    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;
};

/// Factory function to create a database connection object depending on dbConfig.type
std::unique_ptr<IDBConnection> createDBConnection(const DatabaseConfig& dbConfig);
