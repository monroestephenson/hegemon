#include <gtest/gtest.h>
#include "cli.hpp"

TEST(CLIParserTest, NoArgs) {
    int argc = 1;
    const char* argv[] = {"my_db_backup_cli"};
    CLIOptions opts = parseCLI(argc, (char**)argv);
    EXPECT_TRUE(opts.command.empty());
    EXPECT_FALSE(opts.verbose);
    EXPECT_EQ(opts.configPath, "config.json");
}

TEST(CLIParserTest, BackupCommandVerboseAndConfig) {
    int argc = 5;
    const char* argv[] = {
        "my_db_backup_cli",
        "backup",
        "--verbose",
        "--config",
        "myconfig.json"
    };
    CLIOptions opts = parseCLI(argc, (char**)argv);
    EXPECT_EQ(opts.command, "backup");
    EXPECT_TRUE(opts.verbose);
    EXPECT_EQ(opts.configPath, "myconfig.json");
}
