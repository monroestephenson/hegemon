#pragma once

#include <string>
#include <map>

namespace dbbackup {

struct DatabaseConfig {
    std::string type;
    std::string host;
    int port = 0;
    std::string username;
    std::string password;
    std::string database;
};

// Forward declare BackupConfig
struct BackupConfig;

struct StorageConfig {
    std::string localPath;
    std::string cloudProvider;
    std::string cloudPath;
    BackupConfig* backup = nullptr;  // Pointer to backup config for retention settings
};

struct LoggingConfig {
    std::string logPath;      // Path to log file
    std::string logLevel;     // Log level (e.g., "debug", "info", "warn", "error")
    bool enableNotifications = false;
    std::string notificationEndpoint;
};

struct CompressionConfig {
    bool enabled = false;
    std::string format = "gzip";  // gzip, bzip2, xz
    std::string level = "medium"; // low, medium, high
};

struct RetentionConfig {
    int days = 30;           // Number of days to keep backups
    int maxBackups = 10;     // Maximum number of backups to keep
};

struct ScheduleConfig {
    bool enabled = false;
    std::string cron = "0 0 * * *"; // Default: daily at midnight
};

struct BackupConfig {
    CompressionConfig compression;
    RetentionConfig retention;
    ScheduleConfig schedule;
};

struct EncryptionConfig {
    bool enabled = false;
    std::string algorithm = "AES-256-GCM";
    std::string keyPath;
};

struct SecurityConfig {
    EncryptionConfig encryption;
};

class Config {
public:
    DatabaseConfig database;
    StorageConfig storage;
    LoggingConfig logging;
    BackupConfig backup;
    SecurityConfig security;

    static Config fromFile(const std::string& configPath);

private:
    static std::string substituteEnvVars(const std::string& value, bool required = true);
}; 

} // namespace dbbackup 