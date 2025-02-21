#include "mongodb_connection.hpp"
#include <sstream>
#include <filesystem>
#include <stdexcept>

#ifdef USE_MONGODB
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#endif

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

bool MongoDBConnection::connect(const DatabaseConfig& dbConfig) {
#ifdef USE_MONGODB
    if (connected) {
        return true;
    }

    std::stringstream uri;
    uri << "mongodb://"
        << dbConfig.username << ":" << dbConfig.password
        << "@" << dbConfig.host << ":" << dbConfig.port
        << "/" << dbConfig.dbName;

    try {
        mongocxx::v_noabi::uri mongodb_uri(uri.str());
        client = std::make_unique<mongocxx::v_noabi::client>(mongodb_uri);
        
        // Test connection by listing databases
        client->list_databases();
        
        connected = true;
        currentDbName = dbConfig.dbName;
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to connect to MongoDB: ") + e.what());
    }
#else
    throw std::runtime_error("MongoDB support not enabled");
#endif
}

bool MongoDBConnection::disconnect() {
#ifdef USE_MONGODB
    if (!connected) {
        return true;
    }

    client.reset();
    connected = false;
    return true;
#else
    throw std::runtime_error("MongoDB support not enabled");
#endif
}

bool MongoDBConnection::createBackup(const std::string& backupPath) {
#ifdef USE_MONGODB
    if (!connected) {
        throw std::runtime_error("Not connected to MongoDB server");
    }

    // Create the backup directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(backupPath).parent_path());

    // Construct the mongodump command
    std::stringstream cmd;
    cmd << "mongodump"
        << " --host " << client->uri().hosts()[0].name
        << " --port " << client->uri().hosts()[0].port
        << " --username " << client->uri().username()
        << " --password '" << "********" << "'" // Password should be handled securely
        << " --db " << currentDbName
        << " --out " << backupPath
        << " --gzip";  // Compress the output

    int result = system(cmd.str().c_str());
    return result == 0;
#else
    throw std::runtime_error("MongoDB support not enabled");
#endif
}

bool MongoDBConnection::restoreBackup(const std::string& backupPath) {
#ifdef USE_MONGODB
    if (!connected) {
        throw std::runtime_error("Not connected to MongoDB server");
    }

    if (!std::filesystem::exists(backupPath)) {
        throw std::runtime_error("Backup directory does not exist: " + backupPath);
    }

    // Construct the mongorestore command
    std::stringstream cmd;
    cmd << "mongorestore"
        << " --host " << client->uri().hosts()[0].name
        << " --port " << client->uri().hosts()[0].port
        << " --username " << client->uri().username()
        << " --password '" << "********" << "'" // Password should be handled securely
        << " --db " << currentDbName
        << " --drop"  // Drop existing collections before restore
        << " --gzip"  // Input files are compressed
        << " " << backupPath << "/" << currentDbName;

    int result = system(cmd.str().c_str());
    return result == 0;
#else
    throw std::runtime_error("MongoDB support not enabled");
#endif
} 