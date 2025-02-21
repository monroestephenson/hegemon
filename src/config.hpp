#pragma once

#include <string>
#include <vector>
#include <map>

struct DatabaseConfig {
    std::string type;      // e.g., "mysql", "postgres", "mongodb", "sqlite"
    std::string host;
    int         port;
    std::string username;
    std::string password;
    std::string dbName;
    std::string path;      // For SQLite file path
};

struct StorageConfig {
    bool useLocal = true;
    std::string localPath;
    bool useCloud = false;
    std::string cloudProvider; // "aws", "gcp", "azure", etc.
    std::map<std::string, std::string> cloudCredentials; // access_key, secret_key, etc.
};

struct LoggingConfig {
    bool enableLogging = true;
    std::string logFile;
    bool enableSlack = false;
    std::string slackWebhookUrl;
};

struct BackupSchedule {
    bool enabled = false;
    // A simple example schedule (cron-like or simplified)
    std::string cronExpression; 
};

struct Config {
    DatabaseConfig database;
    StorageConfig storage;
    LoggingConfig logging;
    BackupSchedule schedule;
};

/// Loads and parses a JSON config file, returning a Config struct.
/// Throws on parse/validation errors.
Config loadConfig(const std::string& filePath);
