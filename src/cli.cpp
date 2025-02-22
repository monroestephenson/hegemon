#include "cli.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using namespace dbbackup::error;

CLI::CLI(int argc, char* argv[]) : argc(argc), argv(argv) {}

void CLI::printUsage() {
    std::cout << "Hegemon - Database Management CLI\n\n"
              << "Usage:\n"
              << "  " << argv[0] << " <command> [database] [options]\n\n"
              << "Commands:\n"
              << "  backup, -backup      Create a new backup\n"
              << "  restore, -restore    Restore from a backup\n"
              << "  list, -list         List available backups\n"
              << "  verify, -verify     Verify a backup file\n\n"
              << "Database Types:\n"
              << "  mysql               MySQL database\n"
              << "  postgres            PostgreSQL database\n"
              << "  sqlite              SQLite database\n\n"
              << "Options:\n"
              << "  -c, --config <path>    Path to config file (default: ~/.config/hegemon/<db>_config.json)\n"
              << "  -t, --type <type>      Backup type: full|incremental (default: full)\n"
              << "  -h, --host <host>      Database host (default: localhost)\n"
              << "  -p, --port <port>      Database port (default: db-specific)\n"
              << "  -n, --name <dbname>    Database name\n"
              << "  -u, --user <user>      Database username\n"
              << "  -f, --file <path>      SQLite database file path\n"
              << "  --verbose              Enable verbose output\n"
              << "  --help                 Show this help message\n\n"
              << "Examples:\n"
              << "  # Simple MySQL backup:\n"
              << "  " << argv[0] << " backup mysql\n\n"
              << "  # MySQL backup with options:\n"
              << "  " << argv[0] << " backup mysql -n mydb -u myuser\n\n"
              << "  # SQLite backup:\n"
              << "  " << argv[0] << " backup sqlite -f /path/to/db.sqlite\n\n"
              << "  # PostgreSQL backup with custom config:\n"
              << "  " << argv[0] << " backup postgres -c ~/.config/hegemon/custom_config.json\n\n"
              << "  # Restore from backup:\n"
              << "  " << argv[0] << " restore backup_20240222.dump.gz\n\n"
              << "  # List backups:\n"
              << "  " << argv[0] << " list\n";
}

CLIOptions CLI::parse() {
    DB_TRY_CATCH_LOG("CLI", {
        CLIOptions options;

        if (argc < 2) {
            printUsage();
            exit(0);
        }

        // Parse command (first argument)
        std::string cmd = argv[1];
        if (cmd == "--help" || cmd == "-h" || cmd == "help") {
            printUsage();
            exit(0);
        }
        
        // Remove leading dash if present
        if (!cmd.empty() && cmd[0] == '-') {
            cmd = cmd.substr(1);
        }
        
        // Set command
        if (cmd == "backup") {
            options.command = "backup";
            // For backup command, next argument could be database type
            if (argc > 2) {
                std::string dbType = argv[2];
                if (dbType == "mysql" || dbType == "postgres" || dbType == "sqlite") {
                    options.dbType = dbType;
                    // Set default config path based on database type
                    options.configPath = std::string(getenv("HOME")) + "/.config/hegemon/" + dbType + "_config.json";
                }
            }
        }
        else if (cmd == "restore") {
            options.command = "restore";
            // For restore command, next argument is the backup file path
            if (argc > 2 && argv[2][0] != '-') {
                options.restorePath = argv[2];
                // Skip this argument in the option parsing loop
                options.skipArg2 = true;
            }
        }
        else if (cmd == "list") {
            options.command = "list";
            // For list command, next argument could be database type
            if (argc > 2 && argv[2][0] != '-') {
                std::string dbType = argv[2];
                if (dbType == "mysql" || dbType == "postgres" || dbType == "sqlite") {
                    options.dbType = dbType;
                    // Set default config path based on database type
                    options.configPath = std::string(getenv("HOME")) + "/.config/hegemon/" + dbType + "_config.json";
                }
            }
        }
        else if (cmd == "verify") {
            options.command = "verify";
            // For verify command, next argument is the backup file path
            if (argc > 2 && argv[2][0] != '-') {
                options.restorePath = argv[2];
                // Skip this argument in the option parsing loop
                options.skipArg2 = true;
            }
        }
        else {
            DB_THROW(ValidationError, "Unknown command: " + cmd);
        }

        // Parse remaining options
        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];
            
            // Skip if this is a database type argument we already processed
            if (i == 2 && (
                (arg[0] != '-' && (options.command == "backup" || options.command == "list")) ||
                options.skipArg2
            )) {
                continue;
            }
            
            if ((arg == "-t" || arg == "--type") && i + 1 < argc) {
                options.backupType = argv[++i];
                if (options.backupType != "full" && options.backupType != "incremental") {
                    DB_THROW(ValidationError, "Invalid backup type. Must be 'full' or 'incremental'");
                }
            }
            else if ((arg == "-h" || arg == "--host") && i + 1 < argc) {
                options.dbHost = argv[++i];
            }
            else if ((arg == "-p" || arg == "--port") && i + 1 < argc) {
                options.dbPort = std::stoi(argv[++i]);
            }
            else if ((arg == "-n" || arg == "--name") && i + 1 < argc) {
                options.dbName = argv[++i];
            }
            else if ((arg == "-u" || arg == "--user") && i + 1 < argc) {
                options.dbUser = argv[++i];
            }
            else if ((arg == "-f" || arg == "--file") && i + 1 < argc) {
                options.dbFile = argv[++i];
            }
            else if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
                options.configPath = argv[++i];
            }
            else if (arg == "--verbose") {
                options.verbose = true;
            }
            else {
                DB_THROW(ValidationError, "Unknown option: " + arg);
            }
        }

        // Validate required options based on command
        if (options.command == "backup") {
            // If no database type specified, try to get it from config
            if (options.dbType.empty() && !std::filesystem::exists(options.configPath)) {
                DB_THROW(ValidationError, "Database type required when no config file exists");
            }
            
            // If database type is sqlite, validate file path
            if (options.dbType == "sqlite") {
                if (!options.dbFile.empty() && !std::filesystem::exists(options.dbFile)) {
                    DB_THROW(ValidationError, "SQLite database file not found: " + options.dbFile);
                }
            }
        }
        else if (options.command == "restore" || options.command == "verify") {
            if (options.restorePath.empty()) {
                DB_THROW(ValidationError, "Backup file path is required for restore/verify");
            }
            if (!std::filesystem::exists(options.restorePath)) {
                DB_THROW(ValidationError, "Backup file not found: " + options.restorePath);
            }
        }

        return options;
    });
    
    return CLIOptions();
}

CLIOptions parseCLI(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    CLIOptions options;
    options.configPath = "config.json"; // Default config path

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--config" && i + 1 < args.size()) {
            options.configPath = args[++i];
        } else if (arg == "backup") {
            options.command = "backup";
        } else if (arg == "restore") {
            options.command = "restore";
        }
    }

    return options;
}
