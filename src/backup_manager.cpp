#include "backup_manager.hpp"
#include "db_connection.hpp"
#include "compression.hpp"
#include "storage.hpp"
#include "logging.hpp"
#include "notifications.hpp"

#include <iostream>
#include <chrono>
#include <ctime>

BackupManager::BackupManager(const Config& cfg)
    : m_config(cfg)
{
}

BackupManager::~BackupManager() = default;

bool BackupManager::backup(const std::string& backupType) {
    // Logging
    auto logger = getLogger();
    logger->info("Starting {} backup...", backupType);

    // Step 1: Connect to DB
    auto conn = createDBConnection(m_config.database);
    if(!conn->connect(m_config.database)) {
        logger->error("Database connection failed.");
        sendNotificationIfNeeded(m_config.logging, "Backup failed: DB connection error.");
        return false;
    }
    
    // Step 2: Prepare backup file path
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    char timeBuf[128];
    strftime(timeBuf, sizeof(timeBuf), "%Y%m%d_%H%M%S", &tm);
    std::string backupFileName = "backup_" + std::string(timeBuf) + "_" + backupType + ".dump";
    std::string localBackupPath = m_config.storage.localPath + "/" + backupFileName;

    // Step 3: Perform DB-specific backup
    // For incremental/differential, you’d maintain a position or metadata.
    bool success = conn->createBackup(localBackupPath);
    if(!success) {
        logger->error("Backup creation failed.");
        conn->disconnect();
        sendNotificationIfNeeded(m_config.logging, "Backup failed: createBackup error.");
        return false;
    }

    // Step 4: Compress the backup
    std::string compressedFilePath = localBackupPath + ".gz";
    if(!compressFile(localBackupPath, compressedFilePath)) {
        logger->warn("Compression failed, proceeding with uncompressed file.");
    } else {
        // Remove original uncompressed if compression success
        // std::remove(localBackupPath.c_str()); // optional
        localBackupPath = compressedFilePath;
    }

    // Step 5: Store the backup (local or cloud)
    if(!storeBackup(m_config.storage, localBackupPath)) {
        logger->error("Storage step failed.");
        conn->disconnect();
        sendNotificationIfNeeded(m_config.logging, "Backup failed: storeBackup error.");
        return false;
    }

    // Step 6: Disconnect DB
    conn->disconnect();

    // Step 7: Logging & Notification
    logger->info("Backup completed successfully: {}", localBackupPath);
    sendNotificationIfNeeded(m_config.logging, "Backup succeeded: " + localBackupPath);

    return true;
}
