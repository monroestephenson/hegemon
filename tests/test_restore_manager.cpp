#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "restore_manager.hpp"
#include "mocks/mock_db_connection.hpp"
#include "config.hpp"

using ::testing::_;
using ::testing::Return;
using namespace dbbackup;

class TestableRestoreManager : public RestoreManager {
public:
    TestableRestoreManager(const dbbackup::Config& cfg, std::unique_ptr<IDBConnection> dbconn)
        : RestoreManager(cfg), mockConn(std::move(dbconn)) {}

protected:
    std::unique_ptr<IDBConnection> createConnection() {
        return std::move(mockConn);
    }

private:
    std::unique_ptr<IDBConnection> mockConn;
};

TEST(RestoreManagerTest, RestoreSuccess) {
    // Arrange
    dbbackup::Config cfg;
    cfg.database.type = "mysql";
    cfg.database.host = "localhost";
    cfg.database.port = 3306;
    cfg.database.username = "test_user";
    cfg.database.password = "test_pass";
    cfg.database.database = "test_db";

    auto mockConn = std::make_unique<MockDBConnection>();
    EXPECT_CALL(*mockConn, connect(::testing::An<const dbbackup::DatabaseConfig&>())).WillOnce(Return(true));
    EXPECT_CALL(*mockConn, restoreBackup("backup.dump")).WillOnce(Return(true));
    EXPECT_CALL(*mockConn, disconnect()).WillOnce(Return(true));

    TestableRestoreManager rm(cfg, std::move(mockConn));

    // Act
    bool result = rm.restore("backup.dump");

    // Assert
    EXPECT_TRUE(result);
}
