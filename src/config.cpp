#include "config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <nlohmann/json.hpp> // Requires nlohmann/json

using json = nlohmann::json;

Config loadConfig(const std::string& filePath) {
    std::ifstream ifs(filePath);
    if(!ifs.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filePath);
    }

    json j;
    ifs >> j;

    Config cfg;

    // Database
    if (j.contains("database")) {
        cfg.database.type     = j["database"].value("type", "mysql");
        cfg.database.host     = j["database"].value("host", "localhost");
        cfg.database.port     = j["database"].value("port", 3306);
        cfg.database.username = j["database"].value("username", "root");
        cfg.database.password = j["database"].value("password", "");
        cfg.database.dbName   = j["database"].value("dbName", "");
        cfg.database.path     = j["database"].value("path", "");
    }

    // Storage
    if (j.contains("storage")) {
        cfg.storage.useLocal  = j["storage"].value("useLocal", true);
        cfg.storage.localPath = j["storage"].value("localPath", "./backups");
        cfg.storage.useCloud  = j["storage"].value("useCloud", false);
        cfg.storage.cloudProvider = j["storage"].value("cloudProvider", "");
        if (j["storage"].contains("cloudCredentials")) {
            for (auto& el : j["storage"]["cloudCredentials"].items()) {
                cfg.storage.cloudCredentials[el.key()] = el.value();
            }
        }
    }

    // Logging
    if (j.contains("logging")) {
        cfg.logging.enableLogging  = j["logging"].value("enableLogging", true);
        cfg.logging.logFile       = j["logging"].value("logFile", "backup.log");
        cfg.logging.enableSlack    = j["logging"].value("enableSlack", false);
        cfg.logging.slackWebhookUrl= j["logging"].value("slackWebhookUrl", "");
    }

    // Schedule
    if (j.contains("schedule")) {
        cfg.schedule.enabled         = j["schedule"].value("enabled", false);
        cfg.schedule.cronExpression  = j["schedule"].value("cronExpression", "");
    }

    return cfg;
}
