#include "config.hpp"
#include "error/ErrorUtils.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <regex>
#include <cstdlib>

using json = nlohmann::json;
using namespace dbbackup::error;

namespace dbbackup {

std::string Config::substituteEnvVars(const std::string& value) {
    std::regex env_var_pattern("\\$\\{([A-Za-z_][A-Za-z0-9_]*)\\}");
    std::string result = value;
    
    std::smatch match;
    std::string::const_iterator searchStart = result.cbegin();
    while (std::regex_search(searchStart, result.cend(), match, env_var_pattern)) {
        std::string env_var_name = match[1].str();
        const char* env_var_value = std::getenv(env_var_name.c_str());
        
        if (!env_var_value) {
            DB_THROW(ConfigurationError, "Environment variable not set: " + env_var_name);
        }
        
        auto replaceStart = result.begin() + (searchStart - result.begin()) + match.position();
        result.replace(replaceStart, replaceStart + match.length(), env_var_value);
        searchStart = result.begin() + (replaceStart - result.begin()) + strlen(env_var_value);
    }
    
    return result;
}

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
        config.database.type = dbConfig["type"].get<std::string>();
        
        // SQLite only needs the database file path
        if (config.database.type == "sqlite") {
            DB_CHECK(dbConfig.contains("database"), ConfigurationError, "Missing SQLite database file path");
            config.database.database = substituteEnvVars(dbConfig["database"].get<std::string>());
        } else {
            // Other databases need host and port
            DB_CHECK(dbConfig.contains("host"), ConfigurationError, "Missing database host");
            DB_CHECK(dbConfig.contains("port"), ConfigurationError, "Missing database port");
            
            config.database.host = dbConfig["host"].get<std::string>();
            config.database.port = dbConfig["port"].get<int>();
            
            if (dbConfig.contains("username")) {
                config.database.username = substituteEnvVars(dbConfig["username"].get<std::string>());
            }
            if (dbConfig.contains("password")) {
                config.database.password = substituteEnvVars(dbConfig["password"].get<std::string>());
            }
            if (dbConfig.contains("database")) {
                config.database.database = dbConfig["database"].get<std::string>();
            }
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
            config.logging.notificationEndpoint = substituteEnvVars(loggingConfig["notificationEndpoint"].get<std::string>());
        }

        // Backup configuration
        if (configJson.contains("backup")) {
            const auto& backupConfig = configJson["backup"];
            
            // Compression settings
            if (backupConfig.contains("compression")) {
                const auto& compressionConfig = backupConfig["compression"];
                config.backup.compression.enabled = compressionConfig.value("enabled", false);
                config.backup.compression.format = compressionConfig.value("format", "gzip");
                config.backup.compression.level = compressionConfig.value("level", "medium");
            }
            
            // Retention settings
            if (backupConfig.contains("retention")) {
                const auto& retentionConfig = backupConfig["retention"];
                config.backup.retention.days = retentionConfig.value("days", 30);
                config.backup.retention.maxBackups = retentionConfig.value("maxBackups", 10);
            }
            
            // Schedule settings
            if (backupConfig.contains("schedule")) {
                const auto& scheduleConfig = backupConfig["schedule"];
                config.backup.schedule.enabled = scheduleConfig.value("enabled", false);
                config.backup.schedule.cron = scheduleConfig.value("cron", "0 0 * * *");
            }

            // Set up the backup pointer in storage config
            config.storage.backup = &config.backup;
        }

        // Security configuration
        if (configJson.contains("security")) {
            const auto& securityConfig = configJson["security"];
            
            if (securityConfig.contains("encryption")) {
                const auto& encryptionConfig = securityConfig["encryption"];
                config.security.encryption.enabled = encryptionConfig.value("enabled", false);
                config.security.encryption.algorithm = encryptionConfig.value("algorithm", "AES-256-GCM");
                
                if (encryptionConfig.contains("keyPath")) {
                    config.security.encryption.keyPath = substituteEnvVars(encryptionConfig["keyPath"].get<std::string>());
                }
            }
        }

        // Validate required fields
        DB_CHECK(!config.database.type.empty(), ConfigurationError, "Database type cannot be empty");
        if (config.database.type != "sqlite") {
            DB_CHECK(!config.database.host.empty(), ConfigurationError, "Database host cannot be empty");
            DB_CHECK(config.database.port > 0, ConfigurationError, "Invalid database port");
        } else {
            DB_CHECK(!config.database.database.empty(), ConfigurationError, "SQLite database file path cannot be empty");
        }
        DB_CHECK(!config.storage.localPath.empty(), ConfigurationError, "Storage local path cannot be empty");
        DB_CHECK(!config.logging.logPath.empty(), ConfigurationError, "Log path cannot be empty");
        DB_CHECK(!config.logging.logLevel.empty(), ConfigurationError, "Log level cannot be empty");

        // Validate backup configuration
        if (config.backup.compression.enabled) {
            DB_CHECK(config.backup.compression.format == "gzip" || 
                    config.backup.compression.format == "bzip2" ||
                    config.backup.compression.format == "xz",
                    ConfigurationError, "Invalid compression format");
            
            DB_CHECK(config.backup.compression.level == "low" ||
                    config.backup.compression.level == "medium" ||
                    config.backup.compression.level == "high",
                    ConfigurationError, "Invalid compression level");
        }

        // Validate schedule configuration
        if (config.backup.schedule.enabled) {
            std::regex cron_pattern("^(\\*|[0-9,\\-\\*/]+)\\s+(\\*|[0-9,\\-\\*/]+)\\s+(\\*|[0-9,\\-\\*/]+)\\s+(\\*|[0-9,\\-\\*/]+)\\s+(\\*|[0-9,\\-\\*/]+)$");
            DB_CHECK(std::regex_match(config.backup.schedule.cron, cron_pattern),
                    ConfigurationError, "Invalid cron expression");
        }

        return config;
    });

    // This return is never reached due to DB_TRY_CATCH_LOG, but needed for compilation
    return Config();
}

} // namespace dbbackup
