#include "config.hpp"
#include "error/ErrorUtils.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace dbbackup::error;

Config Config::fromFile(const std::string& configPath) {
    DB_TRY_CATCH_LOG("Config", {
        // Check if file exists
        std::ifstream configFile(configPath);
        DB_CHECK(configFile.good(), ConfigurationError, "Config file not found: " + configPath);

        // Parse JSON
        json configJson;
        try {
            configFile >> configJson;
        } catch (const json::parse_error& e) {
            DB_THROW(ConfigurationError, "Failed to parse config file: " + std::string(e.what()));
        }

        Config config;

        // Database configuration
        DB_CHECK(configJson.contains("database"), ConfigurationError, "Missing 'database' section in config");
        const auto& dbConfig = configJson["database"];
        
        DB_CHECK(dbConfig.contains("type"), ConfigurationError, "Missing database type");
        DB_CHECK(dbConfig.contains("host"), ConfigurationError, "Missing database host");
        DB_CHECK(dbConfig.contains("port"), ConfigurationError, "Missing database port");
        
        config.database.type = dbConfig["type"].get<std::string>();
        config.database.host = dbConfig["host"].get<std::string>();
        config.database.port = dbConfig["port"].get<int>();
        
        if (dbConfig.contains("username")) {
            config.database.username = dbConfig["username"].get<std::string>();
        }
        if (dbConfig.contains("password")) {
            config.database.password = dbConfig["password"].get<std::string>();
        }
        if (dbConfig.contains("database")) {
            config.database.database = dbConfig["database"].get<std::string>();
        }

        // Storage configuration
        DB_CHECK(configJson.contains("storage"), ConfigurationError, "Missing 'storage' section in config");
        const auto& storageConfig = configJson["storage"];
        
        DB_CHECK(storageConfig.contains("localPath"), ConfigurationError, "Missing storage local path");
        config.storage.localPath = storageConfig["localPath"].get<std::string>();
        
        if (storageConfig.contains("cloudProvider")) {
            config.storage.cloudProvider = storageConfig["cloudProvider"].get<std::string>();
        }
        if (storageConfig.contains("cloudPath")) {
            config.storage.cloudPath = storageConfig["cloudPath"].get<std::string>();
        }

        // Logging configuration
        DB_CHECK(configJson.contains("logging"), ConfigurationError, "Missing 'logging' section in config");
        const auto& loggingConfig = configJson["logging"];
        
        DB_CHECK(loggingConfig.contains("logPath"), ConfigurationError, "Missing log path");
        DB_CHECK(loggingConfig.contains("logLevel"), ConfigurationError, "Missing log level");
        
        config.logging.logPath = loggingConfig["logPath"].get<std::string>();
        config.logging.logLevel = loggingConfig["logLevel"].get<std::string>();
        
        if (loggingConfig.contains("enableNotifications")) {
            config.logging.enableNotifications = loggingConfig["enableNotifications"].get<bool>();
        }
        if (loggingConfig.contains("notificationEndpoint")) {
            config.logging.notificationEndpoint = loggingConfig["notificationEndpoint"].get<std::string>();
        }

        // Validate required fields
        DB_CHECK(!config.database.type.empty(), ConfigurationError, "Database type cannot be empty");
        DB_CHECK(!config.database.host.empty(), ConfigurationError, "Database host cannot be empty");
        DB_CHECK(config.database.port > 0, ConfigurationError, "Invalid database port");
        DB_CHECK(!config.storage.localPath.empty(), ConfigurationError, "Storage local path cannot be empty");
        DB_CHECK(!config.logging.logPath.empty(), ConfigurationError, "Log path cannot be empty");
        DB_CHECK(!config.logging.logLevel.empty(), ConfigurationError, "Log level cannot be empty");

        return config;
    });

    // This return is never reached due to DB_TRY_CATCH_LOG, but needed for compilation
    return Config();
}
