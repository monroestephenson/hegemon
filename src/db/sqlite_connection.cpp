#include "db/sqlite_connection.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <filesystem>

using namespace dbbackup::error;

SQLiteConnection::SQLiteConnection() noexcept : db(nullptr) {}

SQLiteConnection::~SQLiteConnection() noexcept {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool SQLiteConnection::connect(const dbbackup::DatabaseConfig& dbConfig) {
    DB_TRY_CATCH_LOG("SQLiteConnection", {
        // For SQLite, we use the database field as the path to the database file
        if (dbConfig.database.empty()) {
            DB_THROW(ConfigurationError, "SQLite database path not specified");
        }

        // Store config for later use
        currentConfig = dbConfig;

        // Create the directory if it doesn't exist
        std::filesystem::path dbPath(dbConfig.database);
        if (auto parentPath = dbPath.parent_path(); !parentPath.empty()) {
            std::filesystem::create_directories(parentPath);
        }

        int rc = sqlite3_open(dbConfig.database.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::string error = sqlite3_errmsg(db);
            sqlite3_close(db);
            db = nullptr;
            DB_THROW(ConnectionError, "Failed to open SQLite database: " + error);
        }

        // Enable foreign keys
        char* errorMsg = nullptr;
        rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON", nullptr, nullptr, &errorMsg);
        if (rc != SQLITE_OK) {
            std::string error = errorMsg;
            sqlite3_free(errorMsg);
            sqlite3_close(db);
            db = nullptr;
            DB_THROW(ConnectionError, "Failed to enable foreign keys: " + error);
        }

        currentDatabase = dbConfig.database;
        return true;
    });
    
    return false;
}

bool SQLiteConnection::disconnect() {
    DB_TRY_CATCH_LOG("SQLiteConnection", {
        if (db) {
            int rc = sqlite3_close(db);
            if (rc != SQLITE_OK) {
                DB_THROW(ConnectionError, "Failed to close SQLite database: " + 
                        std::string(sqlite3_errmsg(db)));
            }
            db = nullptr;
        }
        return true;
    });
    
    return false;
}

bool SQLiteConnection::createBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("SQLiteConnection", {
        if (!db) {
            DB_THROW(BackupError, "Not connected to SQLite database");
        }

        // Create the backup directory if it doesn't exist
        std::filesystem::path backupFilePath(backupPath);
        if (auto parentPath = backupFilePath.parent_path(); !parentPath.empty()) {
            std::filesystem::create_directories(parentPath);
        }

        // Open the destination database
        sqlite3* backupDb = nullptr;
        int rc = sqlite3_open(backupPath.c_str(), &backupDb);
        if (rc != SQLITE_OK) {
            std::string error = sqlite3_errmsg(backupDb);
            sqlite3_close(backupDb);
            DB_THROW(BackupError, "Failed to create backup file: " + error);
        }

        // Initialize the backup
        sqlite3_backup* backup = sqlite3_backup_init(backupDb, "main", db, "main");
        if (!backup) {
            std::string error = sqlite3_errmsg(backupDb);
            sqlite3_close(backupDb);
            DB_THROW(BackupError, "Failed to initialize backup: " + error);
        }

        // Perform the backup
        rc = sqlite3_backup_step(backup, -1);
        if (rc != SQLITE_DONE) {
            std::string error = sqlite3_errmsg(backupDb);
            sqlite3_backup_finish(backup);
            sqlite3_close(backupDb);
            DB_THROW(BackupError, "Failed to complete backup: " + error);
        }

        // Finish and clean up
        sqlite3_backup_finish(backup);
        sqlite3_close(backupDb);
        
        return true;
    });
    
    return false;
}

bool SQLiteConnection::restoreBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("SQLiteConnection", {
        if (!db) {
            DB_THROW(RestoreError, "Not connected to SQLite database");
        }

        // Verify backup file exists
        if (!std::filesystem::exists(backupPath)) {
            DB_THROW(RestoreError, "Backup file does not exist: " + backupPath);
        }

        // Open the backup database
        sqlite3* backupDb = nullptr;
        int rc = sqlite3_open(backupPath.c_str(), &backupDb);
        if (rc != SQLITE_OK) {
            std::string error = sqlite3_errmsg(backupDb);
            sqlite3_close(backupDb);
            DB_THROW(RestoreError, "Failed to open backup file: " + error);
        }

        // Initialize the backup (note the swapped source and destination)
        sqlite3_backup* backup = sqlite3_backup_init(db, "main", backupDb, "main");
        if (!backup) {
            std::string error = sqlite3_errmsg(db);
            sqlite3_close(backupDb);
            DB_THROW(RestoreError, "Failed to initialize restore: " + error);
        }

        // Perform the restore
        rc = sqlite3_backup_step(backup, -1);
        if (rc != SQLITE_DONE) {
            std::string error = sqlite3_errmsg(db);
            sqlite3_backup_finish(backup);
            sqlite3_close(backupDb);
            DB_THROW(RestoreError, "Failed to complete restore: " + error);
        }

        // Finish and clean up
        sqlite3_backup_finish(backup);
        sqlite3_close(backupDb);
        
        return true;
    });
    
    return false;
} 