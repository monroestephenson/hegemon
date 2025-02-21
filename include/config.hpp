#pragma once

#include <string>
#include <map>

struct DatabaseConfig {
    std::string type;
    std::string host;
    int port = 0;
    std::string username;
    std::string password;
    std::string database;
};

struct StorageConfig {
    std::string localPath;
    std::string cloudProvider;
    std::string cloudPath;
};

struct LoggingConfig {
    std::string logPath;      // Path to log file
    std::string logLevel;     // Log level (e.g., "debug", "info", "warn", "error")
    bool enableNotifications = false;
    std::string notificationEndpoint;
};

class Config {
public:
    DatabaseConfig database;
    StorageConfig storage;
    LoggingConfig logging;

    static Config fromFile(const std::string& configPath);
}; 