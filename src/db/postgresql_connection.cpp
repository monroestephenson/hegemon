#include "db/postgresql_connection.hpp"
#include "error/ErrorUtils.hpp"
#include "credential_manager.hpp"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <sstream>

using namespace dbbackup::error;
using std::to_string;
using std::string;

PostgreSQLConnection::PostgreSQLConnection() noexcept : conn(nullptr) {
}

PostgreSQLConnection::~PostgreSQLConnection() noexcept {
    if (conn) {
        try {
            if (conn->is_open()) {
                conn->close();
            }
        } catch (...) {
            // Ignore errors during destruction
        }
        conn.reset();
    }
}

bool PostgreSQLConnection::connect(const dbbackup::DatabaseConfig& dbConfig) {
    // Store config for later use in backup/restore
    this->currentConfig = dbConfig;

    DB_TRY_CATCH_LOG("PostgreSQLConnection", {
        // Get password from credential manager
        auto& credManager = CredentialManager::getInstance();
        auto cred = credManager.getCredential(
            dbConfig.credentials.passwordKey,
            CredentialType::Password,
            dbConfig.credentials.preferredSources
        );

        if (!cred) {
            DB_THROW(AuthenticationError, "Failed to retrieve database password");
        }

        // Build connection string
        string connStr = "host=" + dbConfig.host +
                            " port=" + to_string(dbConfig.port) +
                            " dbname=" + dbConfig.database +
                            " user=" + dbConfig.credentials.username;
        
        if (!cred->value.empty()) {
            connStr += " password=" + cred->value;
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
            string error = e.what();
            // Check if it's an authentication error
            if (error.find("role") != string::npos && 
                error.find("does not exist") != string::npos) {
                DB_THROW(AuthenticationError, error);
            } else {
                DB_THROW(ConnectionError, error);
            }
        } catch (const std::exception& e) {
            string error = e.what();
            // Check if it's an authentication error
            if (error.find("role") != string::npos && 
                error.find("does not exist") != string::npos) {
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

        // Get password from credential manager
        auto& credManager = CredentialManager::getInstance();
        auto cred = credManager.getCredential(
            currentConfig.credentials.passwordKey,
            CredentialType::Password,
            currentConfig.credentials.preferredSources
        );

        if (!cred) {
            DB_THROW(AuthenticationError, "Failed to retrieve database password");
        }

        // Create a temporary file for the password
        std::string tempPwFile = backupPath + ".pgpass";
        {
            std::ofstream pwFile(tempPwFile);
            pwFile << currentConfig.host << ":" 
                  << currentConfig.port << ":" 
                  << currentDatabase << ":" 
                  << currentConfig.credentials.username << ":" 
                  << cred->value;
        }
        std::filesystem::permissions(tempPwFile, 
            std::filesystem::perms::owner_read | 
            std::filesystem::perms::owner_write);

        // Set PGPASSFILE environment variable
        std::string oldPgpassfile;
        if (const char* current = std::getenv("PGPASSFILE")) {
            oldPgpassfile = current;
        }
        setenv("PGPASSFILE", tempPwFile.c_str(), 1);

        // Construct pg_dump command without password
        std::string cmd = "pg_dump" +
            std::string(" -h ") + currentConfig.host +
            " -p " + std::to_string(currentConfig.port) +
            " -U " + currentConfig.credentials.username +
            " -d " + currentDatabase +
            " -F p" + // Plain text format
            " -f " + backupPath;

        int result = system(cmd.c_str());

        // Restore old PGPASSFILE if it existed
        if (!oldPgpassfile.empty()) {
            setenv("PGPASSFILE", oldPgpassfile.c_str(), 1);
        } else {
            unsetenv("PGPASSFILE");
        }

        // Always remove the temporary password file
        std::filesystem::remove(tempPwFile);

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

        // Get password from credential manager
        auto& credManager = CredentialManager::getInstance();
        auto cred = credManager.getCredential(
            currentConfig.credentials.passwordKey,
            CredentialType::Password,
            currentConfig.credentials.preferredSources
        );

        if (!cred) {
            DB_THROW(AuthenticationError, "Failed to retrieve database password");
        }

        // Create a temporary file for the password
        std::string tempPwFile = backupPath + ".pgpass";
        {
            std::ofstream pwFile(tempPwFile);
            pwFile << currentConfig.host << ":" 
                  << currentConfig.port << ":" 
                  << currentDatabase << ":" 
                  << currentConfig.credentials.username << ":" 
                  << cred->value;
        }
        std::filesystem::permissions(tempPwFile, 
            std::filesystem::perms::owner_read | 
            std::filesystem::perms::owner_write);

        // Set PGPASSFILE environment variable
        std::string oldPgpassfile;
        if (const char* current = std::getenv("PGPASSFILE")) {
            oldPgpassfile = current;
        }
        setenv("PGPASSFILE", tempPwFile.c_str(), 1);

        // Construct psql command without password
        std::string cmd = "psql" +
            std::string(" -h ") + currentConfig.host +
            " -p " + std::to_string(currentConfig.port) +
            " -U " + currentConfig.credentials.username +
            " -d " + currentDatabase +
            " -f " + backupPath;

        int result = system(cmd.c_str());

        // Restore old PGPASSFILE if it existed
        if (!oldPgpassfile.empty()) {
            setenv("PGPASSFILE", oldPgpassfile.c_str(), 1);
        } else {
            unsetenv("PGPASSFILE");
        }

        // Always remove the temporary password file
        std::filesystem::remove(tempPwFile);

        if (result != 0) {
            DB_THROW(RestoreError, "psql restore failed with error code: " + 
                    std::to_string(result));
        }

        return true;
    });
    
    return false;
} 