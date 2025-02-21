#include "cli.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>

using namespace dbbackup::error;

CLI::CLI(int argc, char* argv[]) : argc(argc), argv(argv) {}

void CLI::printUsage() {
    std::cout << "Usage: " << argv[0] << " [options]\n"
              << "Options:\n"
              << "  --config <path>       Path to configuration file\n"
              << "  --backup <type>       Perform backup (type: full, incremental, differential)\n"
              << "  --restore <path>      Restore from backup file\n"
              << "  --help               Show this help message\n";
}

CLIOptions CLI::parse() {
    DB_TRY_CATCH_LOG("CLI", {
        CLIOptions options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                printUsage();
                exit(0);
            } else if (arg == "--config") {
                if (i + 1 >= argc) {
                    DB_THROW(ValidationError, "Missing value for --config");
                }
                options.configPath = argv[++i];
            } else if (arg == "--backup") {
                if (i + 1 >= argc) {
                    DB_THROW(ValidationError, "Missing value for --backup");
                }
                options.command = "backup";
                options.backupType = argv[++i];
            } else if (arg == "--restore") {
                if (i + 1 >= argc) {
                    DB_THROW(ValidationError, "Missing value for --restore");
                }
                options.command = "restore";
                options.restorePath = argv[++i];
            } else {
                DB_THROW(ValidationError, "Unknown option: " + arg);
            }
        }
        
        // Validate required options
        if (options.configPath.empty()) {
            DB_THROW(ValidationError, "Configuration file path is required (--config)");
        }
        
        if (options.command.empty()) {
            DB_THROW(ValidationError, "No command specified (--backup or --restore)");
        }
        
        return options;
    });
    
    // This return is never reached due to DB_TRY_CATCH_LOG, but needed for compilation
    return CLIOptions();
}
