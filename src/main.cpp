#include "backup_manager.hpp"
#include "config.hpp"
#include "cli.hpp"
#include "logging.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <cstdlib>

using namespace dbbackup::error;

int main(int argc, char* argv[]) {
    try {
        // Initialize logging first
        auto logger = getLogger();
        logger->info("Database Backup Tool Starting...");

        // Parse command line arguments
        DB_TRY_CATCH_LOG("Main", {
            CLI cli(argc, argv);
            auto options = cli.parse();
            
            DB_CHECK(!options.configPath.empty(), ConfigurationError, "Configuration file path not provided");
            
            // Load configuration
            auto config = Config::fromFile(options.configPath);
            
            // Create and run backup manager
            BackupManager manager(config);
            
            if (options.command == "backup") {
                DB_CHECK(!options.backupType.empty(), ValidationError, "Backup type not specified");
                if (!manager.backup(options.backupType)) {
                    DB_THROW(BackupError, "Backup operation failed");
                }
            } else if (options.command == "restore") {
                DB_CHECK(!options.restorePath.empty(), ValidationError, "Restore path not specified");
                if (!manager.restore(options.restorePath)) {
                    DB_THROW(RestoreError, "Restore operation failed");
                }
            } else {
                DB_THROW(ValidationError, "Unknown command: " + options.command);
            }
            
            logger->info("Operation completed successfully");
            return EXIT_SUCCESS;
        });
    } catch (const DatabaseBackupError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_FAILURE;
}
