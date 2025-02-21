#include "restore_manager.hpp"
#include "db_connection.hpp"
#include "compression.hpp"
#include "logging.hpp"
#include "notifications.hpp"

#include <iostream>

using namespace dbbackup;

RestoreManager::RestoreManager(const dbbackup::Config& cfg)
    : m_config(cfg)
{
}

RestoreManager::~RestoreManager() = default;

bool RestoreManager::restore(const std::string& backupFilePath, bool selectiveRestore) {
    auto logger = getLogger();
    logger->info("Starting restore from file: {}", backupFilePath);

    // Potentially check if it's compressed
    // For simplicity, assume anything ending in .gz is compressed
    std::string actualBackupPath = backupFilePath;
    bool isCompressed = false;
    if(backupFilePath.size() > 3 && backupFilePath.substr(backupFilePath.size()-3) == ".gz") {
        isCompressed = true;
    }

    // Decompress if needed
    if(isCompressed) {
        std::string decompressedFilePath = backupFilePath.substr(0, backupFilePath.size()-3);
        if(!dbbackup::decompressFile(backupFilePath, decompressedFilePath)) {
            logger->error("Failed to decompress backup file.");
            sendNotificationIfNeeded(m_config.logging, "Restore failed: decompression error.");
            return false;
        }
        actualBackupPath = decompressedFilePath;
    }

    // Connect to DB
    auto conn = createDBConnection(m_config.database);
    if(!conn->connect(m_config.database)) {
        logger->error("Database connection failed during restore.");
        sendNotificationIfNeeded(m_config.logging, "Restore failed: DB connection error.");
        return false;
    }

    // Perform restore
    if(!conn->restoreBackup(actualBackupPath)) {
        logger->error("Restore operation failed.");
        conn->disconnect();
        sendNotificationIfNeeded(m_config.logging, "Restore failed: restoreBackup error.");
        return false;
    }

    // If selectiveRestore is true, you might have additional logic to only restore certain tables.
    if(selectiveRestore) {
        logger->info("Selective restore not fully implemented in this example.");
    }

    // Disconnect DB
    conn->disconnect();

    logger->info("Restore completed successfully.");
    sendNotificationIfNeeded(m_config.logging, "Restore succeeded.");

    return true;
}
