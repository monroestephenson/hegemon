#pragma once

#include "config.hpp"
#include <string>

/// Stores the backup file according to storage config (local or cloud).
/// Return true on success.
bool storeBackup(const StorageConfig& storageConfig, const std::string& localBackupPath);
