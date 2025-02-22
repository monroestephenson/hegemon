#include "cli.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace dbbackup::error;

CLI::CLI(int argc, char* argv[]) : argc(argc), argv(argv) {}

void CLI::printUsage() {
    std::cout << "Database Backup Tool\n\n"
              << "Usage:\n"
              << "  " << argv[0] << " [command] [options]\n\n"
              << "Commands:\n"
              << "  backup   Create a new backup\n"
              << "  restore  Restore from a backup\n"
              << "  list     List available backups\n"
              << "  verify   Verify a backup file\n\n"
              << "Backup Options:\n"
              << "  --type <full|incremental>  Backup type (default: full)\n"
              << "  --compress <none|gzip>     Compression type (default: gzip)\n"
              << "  --db-type <postgres|mysql|sqlite>  Database type\n"
              << "  --db-host <hostname>       Database host\n"
              << "  --db-port <port>           Database port\n"
              << "  --db-name <dbname>         Database name\n"
              << "  --db-user <username>       Database username\n"
              << "  --db-pass <password>       Database password\n"
              << "  --db-file <path>           SQLite database file path\n\n"
              << "Restore Options:\n"
              << "  --file <path>              Backup file to restore from\n\n"
              << "General Options:\n"
              << "  --config <path>            Path to config file (default: /usr/local/etc/database_backup/config.json)\n"
              << "  --verbose                  Enable verbose output\n"
              << "  --help                     Show this help message\n\n"
              << "Examples:\n"
              << "  " << argv[0] << " backup --db-type postgres --db-name mydb\n"
              << "  " << argv[0] << " restore --file backup_20240222.dump.gz\n"
              << "  " << argv[0] << " backup --db-type sqlite --db-file /path/to/db.sqlite\n";
}

CLIOptions CLI::parse() {
    DB_TRY_CATCH_LOG("CLI", {
        CLIOptions options;
        options.configPath = "/usr/local/etc/database_backup/config.json"; // Default config path
        options.backupType = "full";  // Default backup type
        options.compression = "gzip";  // Default compression

        if (argc < 2) {
            printUsage();
            exit(0);
        }

        std::string command = argv[1];
        
        if (command == "help" || command == "--help") {
            printUsage();
            exit(0);
        }

        if (command == "backup" || command == "restore" || command == "list" || command == "verify") {
            options.command = command;
        } else {
            DB_THROW(ValidationError, "Unknown command: " + command);
        }

        // Parse options
        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--type" && i + 1 < argc) {
                options.backupType = argv[++i];
                if (options.backupType != "full" && options.backupType != "incremental") {
                    DB_THROW(ValidationError, "Invalid backup type. Must be 'full' or 'incremental'");
                }
            }
            else if (arg == "--compress" && i + 1 < argc) {
                options.compression = argv[++i];
                if (options.compression != "none" && options.compression != "gzip") {
                    DB_THROW(ValidationError, "Invalid compression type. Must be 'none' or 'gzip'");
                }
            }
            else if (arg == "--db-type" && i + 1 < argc) {
                options.dbType = argv[++i];
                if (options.dbType != "postgres" && options.dbType != "mysql" && options.dbType != "sqlite") {
                    DB_THROW(ValidationError, "Invalid database type. Must be 'postgres', 'mysql', or 'sqlite'");
                }
            }
            else if (arg == "--db-host" && i + 1 < argc) {
                options.dbHost = argv[++i];
            }
            else if (arg == "--db-port" && i + 1 < argc) {
                options.dbPort = std::stoi(argv[++i]);
            }
            else if (arg == "--db-name" && i + 1 < argc) {
                options.dbName = argv[++i];
            }
            else if (arg == "--db-user" && i + 1 < argc) {
                options.dbUser = argv[++i];
            }
            else if (arg == "--db-pass" && i + 1 < argc) {
                options.dbPass = argv[++i];
            }
            else if (arg == "--db-file" && i + 1 < argc) {
                options.dbFile = argv[++i];
            }
            else if (arg == "--file" && i + 1 < argc) {
                options.restorePath = argv[++i];
            }
            else if (arg == "--config" && i + 1 < argc) {
                options.configPath = argv[++i];
            }
            else if (arg == "--verbose") {
                options.verbose = true;
            }
            else {
                DB_THROW(ValidationError, "Unknown option: " + arg);
            }
        }

        // Validate required options
        if (options.command == "backup") {
            if (options.dbType.empty()) {
                DB_THROW(ValidationError, "Database type (--db-type) is required for backup");
            }
            if (options.dbType == "sqlite") {
                if (options.dbFile.empty()) {
                    DB_THROW(ValidationError, "Database file (--db-file) is required for SQLite");
                }
            } else {
                if (options.dbName.empty()) {
                    DB_THROW(ValidationError, "Database name (--db-name) is required");
                }
            }
        }
        else if (options.command == "restore") {
            if (options.restorePath.empty()) {
                DB_THROW(ValidationError, "Backup file (--file) is required for restore");
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
