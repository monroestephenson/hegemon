#pragma once

#include <string>
#include <map>
#include <vector>

namespace dbbackup {

enum class CredentialSource {
    Environment,
    File,
    KeyStore,
    ConfigFile,
    SSM,
    Vault
};

struct DatabaseCredentials {
    std::string username;
    std::string passwordKey;  // Key to look up password in credential manager
    std::vector<CredentialSource> preferredSources;  // Preferred sources for credentials
};

struct DatabaseConfig {
    std::string type;
    std::string host;
    int port = 0;
    DatabaseCredentials credentials;
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

struct CredentialStoreConfig {
    bool enabled = false;
    std::string type;        // keychain, file, vault, ssm
    std::string path;        // Path or endpoint for credential store
    std::string keyPrefix;   // Prefix for credential keys
    std::map<std::string, std::string> options;  // Additional options
};

struct EncryptionConfig {
    bool enabled = false;
    std::string algorithm = "AES-256-GCM";
    std::string keyPath;
};

struct SecurityConfig {
    EncryptionConfig encryption;
    CredentialStoreConfig credentialStore;
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