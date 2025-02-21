#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "backup_manager.hpp"
#include "config.hpp"
#include "mocks/mock_db_connection.hpp"
#include <filesystem>
#include <fstream>

// Normally you'd provide a way to inject the mock DB connection into BackupManager.
// For demonstration, we might override createDBConnection in test or pass a pointer in the constructor.
using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Invoke;
using namespace dbbackup;

class BackupManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test backup directory
        std::filesystem::create_directories("test_backups");
    }

    void TearDown() override {
        // Clean up test backup directory
        std::filesystem::remove_all("test_backups");
    }
};

TEST_F(BackupManagerTest, SuccessfulFullBackup) {
    // Arrange
    dbbackup::Config cfg;
    cfg.database.type = "mysql";
    cfg.database.host = "localhost";
    cfg.database.port = 3306;
    cfg.database.username = "test_user";
    cfg.database.password = "test_pass";
    cfg.database.database = "test_db";
    
    cfg.storage.localPath = "test_backups";
    cfg.storage.cloudProvider = "local";
    
    cfg.logging.logPath = "test.log";
    cfg.logging.logLevel = "info";

    // Create a MockDBConnection
    auto mockConn = std::make_unique<MockDBConnection>();

    // Setup expectations
    EXPECT_CALL(*mockConn, connect(::testing::An<const dbbackup::DatabaseConfig&>()))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(*mockConn, createBackup(_))
        .Times(1)
        .WillOnce(DoAll(
            Invoke([](const std::string& backupPath) {
                // Create an empty file to simulate backup
                std::ofstream file(backupPath);
                file.close();
                return true;
            }),
            Return(true)
        ));
    EXPECT_CALL(*mockConn, disconnect())
        .Times(1)
        .WillOnce(Return(true));

    // We'll craft a custom BackupManager that accepts an external DBConnection pointer
    class TestableBackupManager : public BackupManager {
    public:
        TestableBackupManager(const dbbackup::Config& c, std::unique_ptr<IDBConnection> conn)
            : BackupManager(c), testConn(std::move(conn)) {}

    protected:
        // Override the connection creation to use our mock
        std::unique_ptr<IDBConnection> createConnection() override {
            return std::move(testConn);
        }

    private:
        std::unique_ptr<IDBConnection> testConn;
    };

    TestableBackupManager manager(cfg, std::move(mockConn));

    // Act & Assert
    EXPECT_TRUE(manager.backup("full"));
}
