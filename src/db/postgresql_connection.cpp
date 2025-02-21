#include "postgresql_connection.hpp"
#include <sstream>
#include <filesystem>
#include <stdexcept>

PostgreSQLConnection::PostgreSQLConnection() : conn(nullptr), connected(false) {}

PostgreSQLConnection::~PostgreSQLConnection() {
    if (connected) {
        disconnect();
    }
}

bool PostgreSQLConnection::connect(const DatabaseConfig& dbConfig) {
    if (connected) {
        return true;
    }

    std::stringstream conninfo;
    conninfo << "host=" << dbConfig.host
             << " port=" << dbConfig.port
             << " dbname=" << dbConfig.dbName
             << " user=" << dbConfig.username
             << " password=" << dbConfig.password;

    conn = PQconnectdb(conninfo.str().c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        std::string error = PQerrorMessage(conn);
        PQfinish(conn);
        throw std::runtime_error("Failed to connect to PostgreSQL: " + error);
    }

    connected = true;
    currentDbName = dbConfig.dbName;
    return true;
}

bool PostgreSQLConnection::disconnect() {
    if (!connected) {
        return true;
    }

    PQfinish(conn);
    conn = nullptr;
    connected = false;
    return true;
}

bool PostgreSQLConnection::createBackup(const std::string& backupPath) {
    if (!connected) {
        throw std::runtime_error("Not connected to PostgreSQL server");
    }

    // Create the backup directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(backupPath).parent_path());

    // Construct the pg_dump command
    std::stringstream cmd;
    cmd << "PGPASSWORD='" << PQpass(conn) << "' "
        << "pg_dump"
        << " --host=" << PQhost(conn)
        << " --port=" << PQport(conn)
        << " --username=" << PQuser(conn)
        << " --dbname=" << currentDbName
        << " --file=" << backupPath
        << " --format=custom"      // Use custom format for maximum flexibility
        << " --blobs"             // Include large objects
        << " --verbose"           // Show progress
        << " --no-password";      // Never prompt for password (we use PGPASSWORD)

    int result = system(cmd.str().c_str());
    return result == 0;
}

bool PostgreSQLConnection::restoreBackup(const std::string& backupPath) {
    if (!connected) {
        throw std::runtime_error("Not connected to PostgreSQL server");
    }

    if (!std::filesystem::exists(backupPath)) {
        throw std::runtime_error("Backup file does not exist: " + backupPath);
    }

    // Construct the pg_restore command
    std::stringstream cmd;
    cmd << "PGPASSWORD='" << PQpass(conn) << "' "
        << "pg_restore"
        << " --host=" << PQhost(conn)
        << " --port=" << PQport(conn)
        << " --username=" << PQuser(conn)
        << " --dbname=" << currentDbName
        << " --verbose"           // Show progress
        << " --no-password"       // Never prompt for password (we use PGPASSWORD)
        << " --clean"            // Clean (drop) database objects before recreating
        << " --if-exists"        // Add IF EXISTS to DROP commands
        << " " << backupPath;

    int result = system(cmd.str().c_str());
    return result == 0;
} 