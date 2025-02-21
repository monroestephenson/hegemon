#pragma once

#include "config.hpp"
#include <string>
#include <vector>

/// Stores the backup file according to storage config (local or cloud).
/// Return true on success.
bool storeBackup(const dbbackup::StorageConfig& storageConfig, const std::string& localBackupPath);

struct BackupMetadata {
    std::string filename;
    std::string timestamp;
    size_t size;
    std::string checksum;
};

class LocalStorage {
public:
    /// Initialize local storage with given configuration
    explicit LocalStorage(const dbbackup::StorageConfig& config);

    /// Store a backup file with rotation policy
    /// Returns metadata of stored backup on success
    BackupMetadata storeBackup(const std::string& sourcePath);

    /// Retrieve a backup file by name
    /// Returns path to the backup file
    std::string retrieveBackup(const std::string& backupName);

    /// List all available backups
    /// Returns vector of backup metadata
    std::vector<BackupMetadata> listBackups() const;

    /// Delete a backup by name
    /// Returns true if successful
    bool deleteBackup(const std::string& backupName);

    /// Clean old backups according to retention policy
    /// Returns number of backups deleted
    size_t cleanOldBackups(size_t keepCount);

    /// Get available storage space
    /// Returns available space in bytes
    size_t getAvailableSpace() const;

private:
    dbbackup::StorageConfig config;
    std::string calculateChecksum(const std::string& filePath) const;
    void ensureStorageDirectory() const;
    void saveMetadata(const BackupMetadata& metadata) const;
    std::vector<BackupMetadata> loadMetadata() const;
};
