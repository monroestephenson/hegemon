#include "storage.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <filesystem> // C++17
#include <fstream>

namespace fs = std::filesystem;
using namespace dbbackup::error;

bool storeBackup(const StorageConfig& storageConfig, const std::string& backupPath) {
    DB_TRY_CATCH_LOG("Storage", {
        // Always store locally
        std::filesystem::path localPath(storageConfig.localPath);
        if (!std::filesystem::exists(localPath)) {
            DB_CHECK(std::filesystem::create_directories(localPath),
                    StorageError, "Failed to create local storage directory");
        }

        // Copy to local storage
        std::filesystem::path sourcePath(backupPath);
        std::filesystem::path destPath = localPath / sourcePath.filename();
        
        if (sourcePath != destPath) {
            std::filesystem::copy_file(sourcePath, destPath, 
                std::filesystem::copy_options::overwrite_existing);
        }

        // Handle cloud storage if configured
        if (!storageConfig.cloudProvider.empty()) {
            // TODO: Implement cloud storage
            // For now, just log that cloud storage is not implemented
            // In the future, this would use the appropriate cloud SDK based on cloudProvider
            // DB_THROW(StorageError, "Cloud storage not implemented yet");
        }

        return true;
    });

    return false; // Only reached if an exception was caught
}
