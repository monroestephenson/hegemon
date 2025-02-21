#include <gtest/gtest.h>
#include "config.hpp"
#include "error/DatabaseBackupError.hpp"
#include <fstream>

using namespace dbbackup;
using namespace dbbackup::error;

TEST(ConfigTest, LoadValidConfig) {
    // Create a temporary JSON file
    std::string tempFile = "temp_test_config.json";
    {
        std::ofstream ofs(tempFile);
        ofs << R"({
            "database": {
                "type": "mysql",
                "host": "localhost",
                "port": 3306,
                "username": "root",
                "password": "my-secret-pw",
                "database": "mydb"
            },
            "storage": {
                "localPath": "/tmp/backups",
                "cloudProvider": "",
                "cloudPath": ""
            },
            "logging": {
                "logPath": "/var/log/db_backup",
                "logLevel": "info",
                "enableNotifications": false
            },
            "backup": {
                "compression": {
                    "enabled": true,
                    "format": "gzip",
                    "level": "medium"
                },
                "retention": {
                    "days": 30,
                    "maxBackups": 10
                },
                "schedule": {
                    "enabled": true,
                    "cron": "0 0 * * *"
                }
            },
            "security": {
                "encryption": {
                    "enabled": false,
                    "algorithm": "AES-256-GCM",
                    "keyPath": ""
                }
            }
        })";
    }

    // Now load with Config::fromFile
    Config cfg = Config::fromFile(tempFile);

    // Test database config
    EXPECT_EQ(cfg.database.type, "mysql");
    EXPECT_EQ(cfg.database.host, "localhost");
    EXPECT_EQ(cfg.database.port, 3306);
    EXPECT_EQ(cfg.database.username, "root");
    EXPECT_EQ(cfg.database.password, "my-secret-pw");
    EXPECT_EQ(cfg.database.database, "mydb");

    // Test storage config
    EXPECT_EQ(cfg.storage.localPath, "/tmp/backups");
    EXPECT_EQ(cfg.storage.cloudProvider, "");
    EXPECT_EQ(cfg.storage.cloudPath, "");

    // Test logging config
    EXPECT_EQ(cfg.logging.logPath, "/var/log/db_backup");
    EXPECT_EQ(cfg.logging.logLevel, "info");
    EXPECT_FALSE(cfg.logging.enableNotifications);

    // Test backup config
    EXPECT_TRUE(cfg.backup.compression.enabled);
    EXPECT_EQ(cfg.backup.compression.format, "gzip");
    EXPECT_EQ(cfg.backup.compression.level, "medium");
    EXPECT_EQ(cfg.backup.retention.days, 30);
    EXPECT_EQ(cfg.backup.retention.maxBackups, 10);
    EXPECT_TRUE(cfg.backup.schedule.enabled);
    EXPECT_EQ(cfg.backup.schedule.cron, "0 0 * * *");

    // Clean up
    remove(tempFile.c_str());
}

TEST(ConfigTest, LoadMissingFileThrows) {
    EXPECT_THROW({
        Config::fromFile("non_existent.json");
    }, ConfigurationError);
}

TEST(ConfigTest, LoadInvalidJsonThrows) {
    // Create a temporary file with invalid JSON
    std::string tempFile = "temp_invalid_config.json";
    {
        std::ofstream ofs(tempFile);
        ofs << R"({
            "database": {
                "type": "mysql",
                "host": "localhost",
                port: invalid,  // Invalid JSON
            }
        })";
    }

    EXPECT_THROW({
        Config::fromFile(tempFile);
    }, ConfigurationError);

    // Clean up
    remove(tempFile.c_str());
}

TEST(ConfigTest, LoadMissingRequiredFieldsThrows) {
    // Create a temporary file missing required fields
    std::string tempFile = "temp_missing_fields_config.json";
    {
        std::ofstream ofs(tempFile);
        ofs << R"({
            "database": {
                "type": "mysql"
                // Missing host and port
            },
            "storage": {
                // Missing localPath
            },
            "logging": {
                // Missing logPath and logLevel
            }
        })";
    }

    EXPECT_THROW({
        Config::fromFile(tempFile);
    }, ConfigurationError);

    // Clean up
    remove(tempFile.c_str());
}

TEST(ConfigTest, EnvironmentVariableSubstitution) {
    // Set test environment variables
    setenv("DB_PASSWORD", "test-password", 1);
    setenv("NOTIFICATION_URL", "http://test-url", 1);

    // Create a temporary JSON file with environment variables
    std::string tempFile = "temp_env_config.json";
    {
        std::ofstream ofs(tempFile);
        ofs << R"({
            "database": {
                "type": "mysql",
                "host": "localhost",
                "port": 3306,
                "username": "root",
                "password": "${DB_PASSWORD}",
                "database": "mydb"
            },
            "storage": {
                "localPath": "/tmp/backups",
                "cloudProvider": "",
                "cloudPath": ""
            },
            "logging": {
                "logPath": "/var/log/db_backup",
                "logLevel": "info",
                "enableNotifications": true,
                "notificationEndpoint": "${NOTIFICATION_URL}"
            },
            "backup": {
                "compression": {
                    "enabled": true,
                    "format": "gzip",
                    "level": "medium"
                },
                "retention": {
                    "days": 30,
                    "maxBackups": 10
                },
                "schedule": {
                    "enabled": true,
                    "cron": "0 0 * * *"
                }
            },
            "security": {
                "encryption": {
                    "enabled": false,
                    "algorithm": "AES-256-GCM",
                    "keyPath": ""
                }
            }
        })";
    }

    // Load config and verify substitution
    Config cfg = Config::fromFile(tempFile);
    EXPECT_EQ(cfg.database.password, "test-password");
    EXPECT_EQ(cfg.logging.notificationEndpoint, "http://test-url");

    // Clean up
    remove(tempFile.c_str());
    unsetenv("DB_PASSWORD");
    unsetenv("NOTIFICATION_URL");
}
