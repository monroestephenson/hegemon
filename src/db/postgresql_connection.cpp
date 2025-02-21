#include "db/postgresql_connection.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <cstdlib>
#include <filesystem>

using namespace dbbackup::error;

bool PostgreSQLConnection::connect(const DatabaseConfig& dbConfig) {
    DB_TRY_CATCH_LOG("PostgreSQLConnection", {
        // Store config for later use in backup/restore
        currentConfig = dbConfig;

        // Build connection string
        std::string connStr = "host=" + dbConfig.host +
                            " port=" + std::to_string(dbConfig.port) +
                            " dbname=" + dbConfig.database +
                            " user=" + dbConfig.username;
        
        if (!dbConfig.password.empty()) {
            connStr += " password=" + dbConfig.password;
        }

        try {
            // Attempt to connect
            conn = std::make_unique<pqxx::connection>(connStr);
            if (!conn->is_open()) {
                DB_THROW(ConnectionError, "Failed to open PostgreSQL connection");
            }
            currentDatabase = dbConfig.database;
            return true;
        } catch (const pqxx::broken_connection& e) {
            std::string error = e.what();
            // Check if it's an authentication error
            if (error.find("role") != std::string::npos && 
                error.find("does not exist") != std::string::npos) {
                DB_THROW(AuthenticationError, error);
            } else {
                DB_THROW(ConnectionError, error);
            }
        } catch (const std::exception& e) {
            std::string error = e.what();
            // Check if it's an authentication error
            if (error.find("role") != std::string::npos && 
                error.find("does not exist") != std::string::npos) {
                DB_THROW(AuthenticationError, error);
            } else {
                DB_THROW(ConnectionError, error);
            }
        }
    });
    
    return false;
}

bool PostgreSQLConnection::disconnect() {
    DB_TRY_CATCH_LOG("PostgreSQLConnection", {
        if (conn) {
            if (conn->is_open()) {
                conn->close();
            }
            conn.reset();
        }
        return true;
    });
    
    return false;
}

bool PostgreSQLConnection::createBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("PostgreSQLConnection", {
        if (!conn || !conn->is_open()) {
            DB_THROW(BackupError, "Not connected to PostgreSQL server");
        }

        // Create the backup directory if it doesn't exist
        std::filesystem::path backupFilePath(backupPath);
        if (auto parentPath = backupFilePath.parent_path(); !parentPath.empty()) {
            std::filesystem::create_directories(parentPath);
        }

        // Construct pg_dump command
        std::string cmd = "PGPASSWORD=" + currentConfig.password + " pg_dump" +
            " -h " + currentConfig.host +
            " -p " + std::to_string(currentConfig.port) +
            " -U " + currentConfig.username +
            " -d " + currentDatabase +
            " -F c" + // Custom format
            " -f " + backupPath;

        int result = system(cmd.c_str());
        if (result != 0) {
            DB_THROW(BackupError, "pg_dump failed with error code: " + 
                    std::to_string(result));
        }

        return true;
    });
    
    return false;
}

bool PostgreSQLConnection::restoreBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("PostgreSQLConnection", {
        if (!conn || !conn->is_open()) {
            DB_THROW(RestoreError, "Not connected to PostgreSQL server");
        }

        // Verify backup file exists
        if (!std::filesystem::exists(backupPath)) {
            DB_THROW(RestoreError, "Backup file does not exist: " + backupPath);
        }

        // Construct pg_restore command
        std::string cmd = "PGPASSWORD=" + currentConfig.password + " pg_restore" +
            " -h " + currentConfig.host +
            " -p " + std::to_string(currentConfig.port) +
            " -U " + currentConfig.username +
            " -d " + currentDatabase +
            " -c" + // Clean (drop) database objects before recreating
            " " + backupPath;

        int result = system(cmd.c_str());
        if (result != 0) {
            DB_THROW(RestoreError, "pg_restore failed with error code: " + 
                    std::to_string(result));
        }

        return true;
    });
    
    return false;
} 