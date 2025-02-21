#include "storage.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>

namespace fs = std::filesystem;
using namespace dbbackup::error;

// Helper function to get current timestamp as string
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

LocalStorage::LocalStorage(const dbbackup::StorageConfig& config) : config(config) {
    ensureStorageDirectory();
}

void LocalStorage::ensureStorageDirectory() const {
    DB_TRY_CATCH_LOG("Storage", {
        fs::path localPath(config.localPath);
        if (!fs::exists(localPath)) {
            DB_CHECK(fs::create_directories(localPath),
                    StorageError, "Failed to create local storage directory");
        }

        // Create metadata directory if it doesn't exist
        fs::path metadataPath = localPath / "metadata";
        if (!fs::exists(metadataPath)) {
            DB_CHECK(fs::create_directories(metadataPath),
                    StorageError, "Failed to create metadata directory");
        }
    });
}

std::string LocalStorage::calculateChecksum(const std::string& filePath) const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        DB_THROW(StorageError, "Failed to open file for checksum calculation");
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        DB_THROW(StorageError, "Failed to create message digest context");
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        DB_THROW(StorageError, "Failed to initialize message digest");
    }

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        if (EVP_DigestUpdate(ctx, buffer, file.gcount()) != 1) {
            EVP_MD_CTX_free(ctx);
            DB_THROW(StorageError, "Failed to update message digest");
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    if (EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
        EVP_MD_CTX_free(ctx);
        DB_THROW(StorageError, "Failed to finalize message digest");
    }

    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

BackupMetadata LocalStorage::storeBackup(const std::string& sourcePath) {
    BackupMetadata metadata;
    DB_TRY_CATCH_LOG("Storage", {
        fs::path source(sourcePath);
        if (!fs::exists(source)) {
            DB_THROW(StorageError, "Source backup file does not exist");
        }

        // Check available space
        size_t requiredSpace = fs::file_size(source) * 1.1; // Add 10% buffer
        if (getAvailableSpace() < requiredSpace) {
            DB_THROW(StorageError, "Insufficient storage space");
        }

        // Generate unique filename with timestamp
        std::string timestamp = getCurrentTimestamp();
        fs::path destPath = fs::path(config.localPath) / 
            (fs::path(source).stem().string() + "_" + timestamp + fs::path(source).extension().string());

        // Copy file
        fs::copy_file(source, destPath, fs::copy_options::overwrite_existing);

        // Create metadata
        metadata.filename = destPath.filename().string();
        metadata.timestamp = timestamp;
        metadata.size = fs::file_size(destPath);
        metadata.checksum = calculateChecksum(destPath.string());
        
        saveMetadata(metadata);

        // Clean old backups if needed
        if (config.backup && config.backup->retention.maxBackups > 0) {
            cleanOldBackups(config.backup->retention.maxBackups);
        }
    });
    return metadata;
}

std::string LocalStorage::retrieveBackup(const std::string& backupName) {
    fs::path backupPath = fs::path(config.localPath) / backupName;
    if (!fs::exists(backupPath)) {
        DB_THROW(StorageError, "Backup file does not exist");
    }
    return backupPath.string();
}

std::vector<BackupMetadata> LocalStorage::listBackups() const {
    return loadMetadata();
}

bool LocalStorage::deleteBackup(const std::string& backupName) {
    DB_TRY_CATCH_LOG("Storage", {
        fs::path backupPath = fs::path(config.localPath) / backupName;
        if (!fs::exists(backupPath)) {
            return false;
        }

        // Remove the file
        fs::remove(backupPath);

        // Update metadata
        auto metadata = loadMetadata();
        metadata.erase(
            std::remove_if(metadata.begin(), metadata.end(),
                [&backupName](const BackupMetadata& m) { return m.filename == backupName; }),
            metadata.end()
        );
        
        // Save updated metadata
        fs::path metadataPath = fs::path(config.localPath) / "metadata" / "backups.json";
        std::ofstream metadataFile(metadataPath);
        if (!metadataFile) {
            DB_THROW(StorageError, "Failed to update metadata after deletion");
        }

        return true;
    });
    return false;
}

size_t LocalStorage::cleanOldBackups(size_t keepCount) {
    DB_TRY_CATCH_LOG("Storage", {
        auto metadata = loadMetadata();
        if (metadata.size() <= keepCount) {
            return 0;
        }

        // Sort by timestamp (newest first)
        std::sort(metadata.begin(), metadata.end(),
            [](const BackupMetadata& a, const BackupMetadata& b) {
                return a.timestamp > b.timestamp;
            });

        size_t deletedCount = 0;
        for (size_t i = keepCount; i < metadata.size(); i++) {
            if (deleteBackup(metadata[i].filename)) {
                deletedCount++;
            }
        }

        return deletedCount;
    });
    return 0;
}

size_t LocalStorage::getAvailableSpace() const {
    DB_TRY_CATCH_LOG("Storage", {
        fs::space_info space = fs::space(config.localPath);
        return space.available;
    });
    return 0;
}

void LocalStorage::saveMetadata(const BackupMetadata& metadata) const {
    DB_TRY_CATCH_LOG("Storage", {
        fs::path metadataPath = fs::path(config.localPath) / "metadata" / "backups.json";
        
        // Load existing metadata
        auto existingMetadata = loadMetadata();
        
        // Add new metadata
        existingMetadata.push_back(metadata);
        
        // Save updated metadata
        std::ofstream metadataFile(metadataPath);
        if (!metadataFile) {
            DB_THROW(StorageError, "Failed to save backup metadata");
        }
        
        // Write metadata as JSON
        metadataFile << "[\n";
        for (size_t i = 0; i < existingMetadata.size(); i++) {
            const auto& m = existingMetadata[i];
            metadataFile << "  {\n";
            metadataFile << "    \"filename\": \"" << m.filename << "\",\n";
            metadataFile << "    \"timestamp\": \"" << m.timestamp << "\",\n";
            metadataFile << "    \"size\": " << m.size << ",\n";
            metadataFile << "    \"checksum\": \"" << m.checksum << "\"\n";
            metadataFile << "  }" << (i < existingMetadata.size() - 1 ? "," : "") << "\n";
        }
        metadataFile << "]\n";
    });
}

std::vector<BackupMetadata> LocalStorage::loadMetadata() const {
    DB_TRY_CATCH_LOG("Storage", {
        fs::path metadataPath = fs::path(config.localPath) / "metadata" / "backups.json";
        if (!fs::exists(metadataPath)) {
            return {};
        }

        std::vector<BackupMetadata> metadata;
        std::ifstream metadataFile(metadataPath);
        if (!metadataFile) {
            DB_THROW(StorageError, "Failed to read backup metadata");
        }

        // Simple JSON parsing (in a real implementation, use a proper JSON library)
        std::string line;
        BackupMetadata current;
        while (std::getline(metadataFile, line)) {
            if (line.find("\"filename\"") != std::string::npos) {
                current.filename = line.substr(line.find(":") + 3);
                current.filename = current.filename.substr(0, current.filename.length() - 2);
            } else if (line.find("\"timestamp\"") != std::string::npos) {
                current.timestamp = line.substr(line.find(":") + 3);
                current.timestamp = current.timestamp.substr(0, current.timestamp.length() - 2);
            } else if (line.find("\"size\"") != std::string::npos) {
                current.size = std::stoull(line.substr(line.find(":") + 2));
            } else if (line.find("\"checksum\"") != std::string::npos) {
                current.checksum = line.substr(line.find(":") + 3);
                current.checksum = current.checksum.substr(0, current.checksum.length() - 2);
                metadata.push_back(current);
            }
        }

        return metadata;
    });
    return {};
}

// Keep the original global function for backward compatibility
bool storeBackup(const dbbackup::StorageConfig& storageConfig, const std::string& localBackupPath) {
    DB_TRY_CATCH_LOG("Storage", {
        LocalStorage storage(storageConfig);
        storage.storeBackup(localBackupPath);
        return true;
    });
    return false;
}
