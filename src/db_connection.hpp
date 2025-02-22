#pragma once

#include "config.hpp"
#include <string>
#include <memory>

namespace dbbackup {

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

/// Factory function to create a database connection object depending on dbConfig.type
std::unique_ptr<IDBConnection> createDBConnection(const DatabaseConfig& dbConfig);

} // namespace dbbackup
