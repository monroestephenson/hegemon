#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "backup_manager.hpp"
#include "config.hpp"
#include "mock_db_connection.hpp"

// Normally you'd provide a way to inject the mock DB connection into BackupManager.
// For demonstration, we might override createDBConnection in test or pass a pointer in the constructor.
using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;

TEST(BackupManagerTest, SuccessfulFullBackup) {
    // Arrange
    Config cfg;
    cfg.database.type = "mysql";
    cfg.storage.localPath = "test_backups";

    // Create a MockDBConnection
    auto mockConn = std::make_unique<MockDBConnection>();

    // Setup expectations
    EXPECT_CALL(*mockConn, connect(_))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(*mockConn, createBackup(_))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(*mockConn, disconnect())
        .Times(1)
        .WillOnce(Return(true));

    // We'll craft a custom BackupManager that accepts an external DBConnection pointer
    class TestableBackupManager : public BackupManager {
    public:
        TestableBackupManager(const Config& c, std::unique_ptr<IDBConnection> conn)
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

    // Act
    bool result = manager.backup("full");

    // Assert
    EXPECT_TRUE(result);
}
