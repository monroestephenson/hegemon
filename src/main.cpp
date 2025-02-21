#include "cli.hpp"
#include "config.hpp"
#include "backup_manager.hpp"
#include "restore_manager.hpp"
#include "scheduling.hpp"
#include "logging.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // 1. Parse CLI
    CLIOptions options;
    try {
        options = parseCLI(argc, argv);
    } catch(const std::exception& e) {
        std::cerr << "Error parsing CLI: " << e.what() << std::endl;
        return 1;
    }

    // 2. Load config
    Config cfg;
    try {
        cfg = loadConfig(options.configPath);
    } catch(const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return 1;
    }

    // 3. Initialize logging (optional: dynamic config)
    auto logger = getLogger();
    logger->info("CLI started. Command: {}", options.command);

    // 4. Dispatch command
    if(options.command == "backup") {
        BackupManager bm(cfg);
        bm.backup("full"); // or parse a --type arg from CLI
    }
    else if(options.command == "restore") {
        // In real usage, you'd parse a --file parameter from CLI
        RestoreManager rm(cfg);
        rm.restore("path/to/backup_file.dump.gz", false);
    }
    else if(options.command == "schedule") {
        // Start or manage scheduling in the background
        static Scheduler scheduler(cfg);
        scheduler.start();

        std::cout << "Scheduler started. Press Enter to stop...\n";
        std::cin.get();
        scheduler.stop();
        std::cout << "Scheduler stopped.\n";
    }
    else {
        std::cout << "No valid command provided. Use 'backup', 'restore', or 'schedule'.\n";
    }

    return 0;
}
