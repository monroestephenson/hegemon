#include "mysql_connection.hpp"
#include <sstream>
#include <filesystem>
#include <stdexcept>

MySQLConnection::MySQLConnection() : mysql(nullptr), connected(false) {
    mysql = mysql_init(nullptr);
    if (!mysql) {
        throw std::runtime_error("Failed to initialize MySQL connection");
    }
}

MySQLConnection::~MySQLConnection() {
    if (connected) {
        disconnect();
    }
    if (mysql) {
        mysql_close(mysql);
    }
}

bool MySQLConnection::connect(const DatabaseConfig& dbConfig) {
    if (connected) {
        return true;
    }

    if (!mysql_real_connect(mysql,
                          dbConfig.host.c_str(),
                          dbConfig.username.c_str(),
                          dbConfig.password.c_str(),
                          dbConfig.dbName.c_str(),
                          dbConfig.port,
                          nullptr,
                          0)) {
        throw std::runtime_error(std::string("Failed to connect to MySQL: ") + mysql_error(mysql));
    }

    connected = true;
    currentDbName = dbConfig.dbName;
    return true;
}

bool MySQLConnection::disconnect() {
    if (!connected) {
        return true;
    }

    mysql_close(mysql);
    mysql = mysql_init(nullptr);
    connected = false;
    return true;
}

bool MySQLConnection::createBackup(const std::string& backupPath) {
    if (!connected) {
        throw std::runtime_error("Not connected to MySQL server");
    }

    // Create the backup directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(backupPath).parent_path());

    // Construct the mysqldump command
    std::stringstream cmd;
    cmd << "mysqldump"
        << " --user=" << currentDbName
        << " --password=" << "********" // Password should be handled securely
        << " --host=localhost"
        << " --port=3306"
        << " --databases " << currentDbName
        << " --result-file=" << backupPath
        << " --single-transaction"  // For consistent backup of InnoDB tables
        << " --routines"           // Include stored procedures and functions
        << " --triggers"           // Include triggers
        << " --events";            // Include events

    int result = system(cmd.str().c_str());
    return result == 0;
}

bool MySQLConnection::restoreBackup(const std::string& backupPath) {
    if (!connected) {
        throw std::runtime_error("Not connected to MySQL server");
    }

    if (!std::filesystem::exists(backupPath)) {
        throw std::runtime_error("Backup file does not exist: " + backupPath);
    }

    // Construct the mysql restore command
    std::stringstream cmd;
    cmd << "mysql"
        << " --user=" << currentDbName
        << " --password=" << "********" // Password should be handled securely
        << " --host=localhost"
        << " --port=3306"
        << " " << currentDbName
        << " < " << backupPath;

    int result = system(cmd.str().c_str());
    return result == 0;
} 