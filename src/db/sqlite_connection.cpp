#include "sqlite_connection.hpp"
#include <filesystem>
#include <stdexcept>

SQLiteConnection::SQLiteConnection() : db(nullptr), connected(false) {}

SQLiteConnection::~SQLiteConnection() {
    if (connected) {
        disconnect();
    }
}

bool SQLiteConnection::connect(const DatabaseConfig& dbConfig) {
    if (connected) {
        return true;
    }

    if (dbConfig.path.empty()) {
        throw std::runtime_error("SQLite database path is required");
    }

    int rc = sqlite3_open(dbConfig.path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("Failed to open SQLite database: " + error);
    }

    connected = true;
    dbPath = dbConfig.path;
    return true;
}

bool SQLiteConnection::disconnect() {
    if (!connected) {
        return true;
    }

    sqlite3_close(db);
    db = nullptr;
    connected = false;
    return true;
}

bool SQLiteConnection::createBackup(const std::string& backupPath) {
    if (!connected) {
        throw std::runtime_error("Not connected to SQLite database");
    }

    // Create the backup directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(backupPath).parent_path());

    // Open the backup database
    sqlite3* backupDb;
    int rc = sqlite3_open(backupPath.c_str(), &backupDb);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(backupDb);
        sqlite3_close(backupDb);
        throw std::runtime_error("Failed to create backup database: " + error);
    }

    // Initialize the backup
    sqlite3_backup* backup = sqlite3_backup_init(backupDb, "main", db, "main");
    if (!backup) {
        std::string error = sqlite3_errmsg(backupDb);
        sqlite3_close(backupDb);
        throw std::runtime_error("Failed to initialize backup: " + error);
    }

    // Perform the backup
    rc = sqlite3_backup_step(backup, -1);
    sqlite3_backup_finish(backup);

    if (rc != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(backupDb);
        sqlite3_close(backupDb);
        throw std::runtime_error("Failed to complete backup: " + error);
    }

    sqlite3_close(backupDb);
    return true;
}

bool SQLiteConnection::restoreBackup(const std::string& backupPath) {
    if (!connected) {
        throw std::runtime_error("Not connected to SQLite database");
    }

    if (!std::filesystem::exists(backupPath)) {
        throw std::runtime_error("Backup file does not exist: " + backupPath);
    }

    // Open the backup database
    sqlite3* backupDb;
    int rc = sqlite3_open(backupPath.c_str(), &backupDb);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(backupDb);
        sqlite3_close(backupDb);
        throw std::runtime_error("Failed to open backup database: " + error);
    }

    // Initialize the backup (restore is just a backup in the opposite direction)
    sqlite3_backup* backup = sqlite3_backup_init(db, "main", backupDb, "main");
    if (!backup) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(backupDb);
        throw std::runtime_error("Failed to initialize restore: " + error);
    }

    // Perform the restore
    rc = sqlite3_backup_step(backup, -1);
    sqlite3_backup_finish(backup);

    if (rc != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(backupDb);
        throw std::runtime_error("Failed to complete restore: " + error);
    }

    sqlite3_close(backupDb);
    return true;
} 