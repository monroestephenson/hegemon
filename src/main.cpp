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

        // Create database configuration
        dbbackup::DatabaseConfig dbConfig;
        if (options.dbType == "postgres") {
            dbConfig.type = "postgresql";
            dbConfig.host = options.dbHost.empty() ? "localhost" : options.dbHost;
            dbConfig.port = options.dbPort == 0 ? 5432 : options.dbPort;
            dbConfig.username = options.dbUser;
            dbConfig.password = options.dbPass;
            dbConfig.database = options.dbName;
        }
        else if (options.dbType == "mysql") {
            dbConfig.type = "mysql";
            dbConfig.host = options.dbHost.empty() ? "localhost" : options.dbHost;
            dbConfig.port = options.dbPort == 0 ? 3306 : options.dbPort;
            dbConfig.username = options.dbUser;
            dbConfig.password = options.dbPass;
            dbConfig.database = options.dbName;
        }
        else if (options.dbType == "sqlite") {
            dbConfig.type = "sqlite";
            dbConfig.database = options.dbFile;  // Use the file path directly
        }

        // Create storage configuration
        dbbackup::StorageConfig storageConfig;
        storageConfig.localPath = "backups";  // Default to local backups directory

        // Create backup configuration
        dbbackup::BackupConfig backupConfig;
        backupConfig.compression.enabled = (options.compression != "none");
        backupConfig.compression.format = "gzip";
        backupConfig.compression.level = "medium";

        // Create logging configuration
        dbbackup::LoggingConfig loggingConfig;
        loggingConfig.logPath = "logs";
        loggingConfig.logLevel = options.verbose ? "debug" : "info";

        // Create complete configuration
        dbbackup::Config config;
        config.database = dbConfig;
        config.storage = storageConfig;
        config.backup = backupConfig;
        config.logging = loggingConfig;

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
