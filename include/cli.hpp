#pragma once

#include <string>

struct CLIOptions {
    std::string command;        // backup, restore, list, verify
    std::string configPath;     // Path to config file
    std::string backupType;     // full, incremental
    std::string compression;    // none, gzip
    std::string dbType;         // postgres, mysql, sqlite
    std::string dbHost;         // Database host
    int dbPort;                // Database port
    std::string dbName;         // Database name
    std::string dbUser;         // Database username
    std::string dbPass;         // Database password
    std::string dbFile;         // SQLite database file path
    std::string restorePath;    // Path to backup file for restore
    bool verbose;               // Enable verbose output

    CLIOptions() : dbPort(0), verbose(false) {
        configPath = "/usr/local/etc/hegemon/config.json"; // Default config path
        backupType = "full";  // Default backup type
        compression = "gzip";  // Default compression
    }
};

class CLI {
public:
    CLI(int argc, char* argv[]);
    CLIOptions parse();

private:
    void printUsage();
    int argc;
    char** argv;
}; 