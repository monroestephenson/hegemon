#include <gtest/gtest.h>
#include "config.hpp"
#include <fstream>

TEST(ConfigTest, LoadValidConfig) {
    // Create a temporary JSON file
    std::string tempFile = "temp_test_config.json";
    {
        std::ofstream ofs(tempFile);
        ofs << R"({
            "database": {
                "type": "mysql",
                "host": "localhost",
                "port": 3306,
                "username": "root",
                "password": "my-secret-pw",
                "dbName": "mydb"
            }
        })";
    }

    // Now load with loadConfig
    Config cfg = loadConfig(tempFile);

    EXPECT_EQ(cfg.database.type, "mysql");
    EXPECT_EQ(cfg.database.host, "localhost");
    EXPECT_EQ(cfg.database.port, 3306);
    EXPECT_EQ(cfg.database.username, "root");
    EXPECT_EQ(cfg.database.password, "my-secret-pw");
    EXPECT_EQ(cfg.database.dbName, "mydb");

    // Clean up
    remove(tempFile.c_str());
}

TEST(ConfigTest, LoadMissingFileThrows) {
    EXPECT_THROW({
        loadConfig("non_existent.json");
    }, std::runtime_error);
}
