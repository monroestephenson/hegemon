#include "db/mysql_connection.hpp"
#include "error/ErrorUtils.hpp"
#include "credential_manager.hpp"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <sstream>

using namespace dbbackup::error;

MySQLConnection::MySQLConnection() noexcept : mysql(nullptr) {
    mysql = mysql_init(nullptr);
    if (!mysql) {
        DB_THROW(ConnectionError, "Failed to initialize MySQL connection");
    }
}

MySQLConnection::~MySQLConnection() noexcept {
    if (mysql) {
        mysql_close(mysql);
        mysql = nullptr;
    }
}

bool MySQLConnection::connect(const dbbackup::DatabaseConfig& dbConfig) {
    DB_TRY_CATCH_LOG("MySQLConnection", {
        if (!mysql) {
            DB_THROW(ConnectionError, "MySQL connection not initialized");
        }

        // Store config for later use in backup/restore
        currentConfig = dbConfig;

        // Set connection timeout to 5 seconds
        int timeout = 5;
        mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

        // Enable automatic reconnection
        bool reconnect = true;
        mysql_options(mysql, MYSQL_OPT_RECONNECT, &reconnect);

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

        // Attempt to connect
        MYSQL* result = mysql_real_connect(mysql,
            dbConfig.host.c_str(),
            dbConfig.credentials.username.c_str(),
            cred->value.c_str(),
            dbConfig.database.c_str(),
            dbConfig.port,
            nullptr,  // unix_socket
            0        // client_flag
        );

        if (!result) {
            std::string error = mysql_error(mysql);
            mysql_close(mysql);
            mysql = mysql_init(nullptr);  // Reinitialize for future attempts

            // Check if it's an authentication error
            if (error.find("Access denied") != std::string::npos) {
                DB_THROW(AuthenticationError, error);
            } else {
                DB_THROW(ConnectionError, error);
            }
        }

        currentDatabase = dbConfig.database;
        return true;
    });
    
    return false;
}

bool MySQLConnection::disconnect() {
    DB_TRY_CATCH_LOG("MySQLConnection", {
        if (mysql) {
            mysql_close(mysql);
            mysql = mysql_init(nullptr);  // Reinitialize for future attempts
        }
        return true;
    });
    
    return false;
}

bool MySQLConnection::createBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("MySQLConnection", {
        if (!mysql || mysql_ping(mysql) != 0) {
            DB_THROW(BackupError, "Not connected to MySQL server");
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
        std::string tempPwFile = backupPath + ".pw";
        {
            std::ofstream pwFile(tempPwFile);
            pwFile << "[client]\n"
                  << "password=" << cred->value << "\n";
        }
        std::filesystem::permissions(tempPwFile, 
            std::filesystem::perms::owner_read | 
            std::filesystem::perms::owner_write);

        // Construct mysqldump command using defaults-extra-file for password
        std::string cmd = "mysqldump"
            " --defaults-extra-file=" + tempPwFile +
            " --host=" + currentConfig.host +
            " --port=" + std::to_string(currentConfig.port) +
            " --user=" + currentConfig.credentials.username +
            " --databases " + currentDatabase +
            " --add-drop-database" +
            " --add-drop-table" +
            " --create-options" +
            " --quote-names" +
            " --single-transaction" +  // For InnoDB tables
            " --set-gtid-purged=OFF" +
            " --result-file=" + backupPath;

        int result = system(cmd.c_str());
        
        // Always remove the temporary password file
        std::filesystem::remove(tempPwFile);

        if (result != 0) {
            DB_THROW(BackupError, "mysqldump failed with error code: " + 
                    std::to_string(result));
        }

        return true;
    });
    
    return false;
}

bool MySQLConnection::restoreBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("MySQLConnection", {
        if (!mysql || mysql_ping(mysql) != 0) {
            DB_THROW(RestoreError, "Not connected to MySQL server");
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
        std::string tempPwFile = backupPath + ".pw";
        {
            std::ofstream pwFile(tempPwFile);
            pwFile << "[client]\n"
                  << "password=" << cred->value << "\n";
        }
        std::filesystem::permissions(tempPwFile, 
            std::filesystem::perms::owner_read | 
            std::filesystem::perms::owner_write);

        // Construct mysql command using defaults-extra-file for password
        std::string cmd = "mysql"
            " --defaults-extra-file=" + tempPwFile +
            " --host=" + currentConfig.host +
            " --port=" + std::to_string(currentConfig.port) +
            " --user=" + currentConfig.credentials.username +
            " " + currentDatabase +
            " < " + backupPath;

        int result = system(cmd.c_str());
        
        // Always remove the temporary password file
        std::filesystem::remove(tempPwFile);

        if (result != 0) {
            DB_THROW(RestoreError, "mysql restore failed with error code: " + 
                    std::to_string(result));
        }

        return true;
    });
    
    return false;
} 