#pragma once

#include "config.hpp"
#include "db_connection.hpp"
#include <string>
#include <memory>

class RestoreManager {
public:
    RestoreManager(const dbbackup::Config& cfg);
    virtual ~RestoreManager();

    /// Restores the database from a given backup file path (compressed or uncompressed).
    bool restore(const std::string& backupFilePath, bool selectiveRestore = false);

protected:
    virtual std::unique_ptr<IDBConnection> createConnection() {
        return createDBConnection(m_config.database);
    }

private:
    dbbackup::Config m_config;
};
