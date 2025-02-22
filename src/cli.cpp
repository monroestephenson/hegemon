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
              << "  " << argv[0] << " [command] [options]\n\n"
              << "Commands:\n"
              << "  backup   Create a new backup\n"
              << "  restore  Restore from a backup\n"
              << "  list     List available backups\n"
              << "  verify   Verify a backup file\n\n"
              << "General Options:\n"
              << "  --config <path>            Path to config file (default: ~/.config/hegemon/config.json)\n"
              << "  --verbose                  Enable verbose output\n"
              << "  --help                     Show this help message\n\n"
              << "Backup Options (can be specified in config file):\n"
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
              << "  --file <path>              Backup file to restore from (required)\n\n"
              << "Examples:\n"
              << "  # Using config file only:\n"
              << "  " << argv[0] << " backup --config ~/.config/hegemon/mysql_config.json\n\n"
              << "  # Overriding config options:\n"
              << "  " << argv[0] << " backup --config ~/.config/hegemon/mysql_config.json --db-name different_db\n\n"
              << "  # Full command without config:\n"
              << "  " << argv[0] << " backup --db-type mysql --db-name mydb --db-host localhost --db-port 3306 --db-user myuser\n\n"
              << "  # Restore command:\n"
              << "  " << argv[0] << " restore --file backup_20240222.dump.gz\n\n"
              << "  # SQLite backup:\n"
              << "  " << argv[0] << " backup --config ~/.config/hegemon/sqlite_config.json\n"
              << "  " << argv[0] << " backup --db-type sqlite --db-file /path/to/db.sqlite\n";
}

CLIOptions CLI::parse() {
    DB_TRY_CATCH_LOG("CLI", {
        CLIOptions options;
        options.configPath = "/usr/local/etc/hegemon/config.json"; // Default config path
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

        // Validate required options based on command
        if (options.command == "backup") {
            // Database type is now optional as it can come from config
            if (!options.dbType.empty()) {
                // If db-type is provided, validate related options
                if (options.dbType == "sqlite") {
                    if (!options.dbFile.empty() && !std::filesystem::exists(options.dbFile)) {
                        DB_THROW(ValidationError, "SQLite database file not found: " + options.dbFile);
                    }
                }
            }
        }
        else if (options.command == "restore") {
            if (options.restorePath.empty()) {
                DB_THROW(ValidationError, "Backup file (--file) is required for restore");
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
