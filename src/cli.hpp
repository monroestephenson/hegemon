#pragma once

#include <string>

/// A simple struct to hold parsed CLI arguments.
struct CLIOptions {
    std::string command;           // e.g. "backup" or "restore" or "schedule"
    std::string configPath;        // Path to config file
    bool verbose = false;
    // Additional fields as needed...
};

/// Parse command-line arguments.
/// Returns a CLIOptions struct with user-supplied parameters.
CLIOptions parseCLI(int argc, char** argv);
