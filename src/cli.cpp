#include "cli.hpp"

#include <iostream>
#include <CLI/CLI.hpp> // Requires the CLI11 library

CLIOptions parseCLI(int argc, char** argv) {
    CLIOptions options;

    CLI::App app{"Database Backup CLI"};

    // Global options
    app.add_flag("-v,--verbose", options.verbose, "Enable verbose logging");
    app.add_option("-c,--config", options.configPath, "Path to configuration file")
        ->default_val("config.json");

    // Subcommands
    auto backupCmd = app.add_subcommand("backup", "Perform a backup");
    auto restoreCmd = app.add_subcommand("restore", "Perform a restore");
    auto scheduleCmd = app.add_subcommand("schedule", "Manage scheduling of backups");

    // You can add more advanced options under each subcommand if needed.

    // Parse
    CLI11_PARSE(app, argc, argv);

    // Determine which command was used
    if (backupCmd->parsed()) {
        options.command = "backup";
    } else if (restoreCmd->parsed()) {
        options.command = "restore";
    } else if (scheduleCmd->parsed()) {
        options.command = "schedule";
    }

    return options;
}
