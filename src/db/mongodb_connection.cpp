#include "mongodb_connection.hpp"
#include "error/ErrorUtils.hpp"
#include <sstream>
#include <filesystem>

#ifdef USE_MONGODB
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#endif

using namespace dbbackup::error;

MongoDBConnection::MongoDBConnection() : connected(false) {
#ifdef USE_MONGODB
    instance = std::make_unique<mongocxx::v_noabi::instance>();
#endif
}

MongoDBConnection::~MongoDBConnection() {
    if (connected) {
        disconnect();
    }
}

bool MongoDBConnection::connect(const dbbackup::DatabaseConfig& dbConfig) {
    DB_TRY_CATCH_LOG("MongoDBConnection", {
#ifdef USE_MONGODB
        if (connected) {
            return true;
        }

        // Store config for later use
        currentConfig = dbConfig;

        std::stringstream uri;
        uri << "mongodb://"
            << dbConfig.username << ":" << dbConfig.password
            << "@" << dbConfig.host << ":" << dbConfig.port
            << "/" << dbConfig.database;

        try {
            mongocxx::v_noabi::uri mongodb_uri(uri.str());
            client = std::make_unique<mongocxx::v_noabi::client>(mongodb_uri);
            
            // Test connection by listing databases
            client->list_databases();
            
            connected = true;
            currentDbName = dbConfig.database;
            return true;
        } catch (const std::exception& e) {
            DB_THROW(ConnectionError, std::string("Failed to connect to MongoDB: ") + e.what());
        }
#else
        DB_THROW(ConfigurationError, "MongoDB support not enabled");
#endif
    });
    return false;
}

bool MongoDBConnection::disconnect() {
    DB_TRY_CATCH_LOG("MongoDBConnection", {
#ifdef USE_MONGODB
        if (!connected) {
            return true;
        }

        client.reset();
        connected = false;
        return true;
#else
        DB_THROW(ConfigurationError, "MongoDB support not enabled");
#endif
    });
    return false;
}

bool MongoDBConnection::createBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("MongoDBConnection", {
#ifdef USE_MONGODB
        if (!connected) {
            DB_THROW(BackupError, "Not connected to MongoDB server");
        }

        // Create the backup directory if it doesn't exist
        std::filesystem::create_directories(std::filesystem::path(backupPath).parent_path());

        // Construct the mongodump command
        std::stringstream cmd;
        cmd << "mongodump"
            << " --host " << client->uri().hosts()[0].name
            << " --port " << client->uri().hosts()[0].port
            << " --username " << client->uri().username()
            << " --password '" << currentConfig.password << "'"
            << " --db " << currentDbName
            << " --out " << backupPath
            << " --gzip";  // Compress the output

        int result = system(cmd.str().c_str());
        if (result != 0) {
            DB_THROW(BackupError, "mongodump failed with error code: " + std::to_string(result));
        }
        return true;
#else
        DB_THROW(ConfigurationError, "MongoDB support not enabled");
#endif
    });
    return false;
}

bool MongoDBConnection::restoreBackup(const std::string& backupPath) {
    DB_TRY_CATCH_LOG("MongoDBConnection", {
#ifdef USE_MONGODB
        if (!connected) {
            DB_THROW(RestoreError, "Not connected to MongoDB server");
        }

        if (!std::filesystem::exists(backupPath)) {
            DB_THROW(RestoreError, "Backup directory does not exist: " + backupPath);
        }

        // Construct the mongorestore command
        std::stringstream cmd;
        cmd << "mongorestore"
            << " --host " << client->uri().hosts()[0].name
            << " --port " << client->uri().hosts()[0].port
            << " --username " << client->uri().username()
            << " --password '" << currentConfig.password << "'"
            << " --db " << currentDbName
            << " --drop"  // Drop existing collections before restore
            << " --gzip"  // Input files are compressed
            << " " << backupPath << "/" << currentDbName;

        int result = system(cmd.str().c_str());
        if (result != 0) {
            DB_THROW(RestoreError, "mongorestore failed with error code: " + std::to_string(result));
        }
        return true;
#else
        DB_THROW(ConfigurationError, "MongoDB support not enabled");
#endif
    });
    return false;
} 