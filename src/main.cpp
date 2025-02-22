#include <cli.hpp>
#include "config.hpp"
#include "backup_manager.hpp"
#include "restore_manager.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <memory>

using namespace dbbackup::error;

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        CLI cli(argc, argv);
        CLIOptions options = cli.parse();

        // Load configuration from file
        dbbackup::Config config = dbbackup::Config::fromFile(options.configPath);

        // Override config with command line options if provided
        if (!options.dbType.empty()) {
            config.database.type = options.dbType == "postgres" ? "postgresql" : options.dbType;
        }
        
        if (!options.dbHost.empty()) {
            config.database.host = options.dbHost;
        }
        
        if (options.dbPort != 0) {
            config.database.port = options.dbPort;
        }
        
        if (!options.dbUser.empty()) {
            config.database.username = options.dbUser;
        }
        
        if (!options.dbName.empty()) {
            config.database.database = options.dbName;
        }
        
        if (!options.dbFile.empty() && config.database.type == "sqlite") {
            config.database.database = options.dbFile;
        }

        // Override backup settings if provided
        if (!options.compression.empty()) {
            config.backup.compression.enabled = (options.compression != "none");
            if (config.backup.compression.enabled) {
                config.backup.compression.format = options.compression;
            }
        }

        // Override logging settings
        if (options.verbose) {
            config.logging.logLevel = "debug";
        }

        // Execute the appropriate command
        if (options.command == "backup") {
            BackupManager backupMgr(config);
            if (!backupMgr.backup(options.backupType)) {
                return 1;
            }
        }
        else if (options.command == "restore") {
            RestoreManager restoreMgr(config);
            if (!restoreMgr.restore(options.restorePath)) {
                return 1;
            }
        }
        else if (options.command == "list") {
            // TODO: Implement listing backups
            std::cout << "Listing backups is not yet implemented\n";
            return 1;
        }
        else if (options.command == "verify") {
            // TODO: Implement backup verification
            std::cout << "Backup verification is not yet implemented\n";
            return 1;
        }
        else {
            std::cerr << "Error: No command specified. Use -h or --help for usage information.\n";
            return 1;
        }

        return 0;
    }
    catch (const DatabaseBackupError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}
