#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "db_connection.hpp"
#include "db/mysql_connection.hpp"
#include "db/postgresql_connection.hpp"
#include "db/sqlite_connection.hpp"

// Only include MongoDB headers when MongoDB support is enabled
#ifdef USE_MONGODB
#include "db/mongodb_connection.hpp"
#endif

class DBConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test configs
        mysqlConfig.type = "mysql";
        mysqlConfig.host = "localhost";
        mysqlConfig.port = 3306;
        mysqlConfig.username = "test_user";
        mysqlConfig.password = "test_pass";
        mysqlConfig.database = "test_db";

        postgresConfig = mysqlConfig;
        postgresConfig.type = "postgresql";
        postgresConfig.port = 5432;

        mongoConfig = mysqlConfig;
        mongoConfig.type = "mongodb";
        mongoConfig.port = 27017;

        sqliteConfig = mysqlConfig;
        sqliteConfig.type = "sqlite";
        sqliteConfig.database = "/tmp/test.db";
    }

    DatabaseConfig mysqlConfig;
    DatabaseConfig postgresConfig;
    DatabaseConfig mongoConfig;
    DatabaseConfig sqliteConfig;
};

// Test the factory function
TEST_F(DBConnectionTest, FactoryCreatesCorrectTypes) {
    auto mysql = createDBConnection(mysqlConfig);
    auto postgres = createDBConnection(postgresConfig);
    auto sqlite = createDBConnection(sqliteConfig);

#ifdef USE_MYSQL
    EXPECT_NE(dynamic_cast<MySQLConnection*>(mysql.get()), nullptr);
#endif

#ifdef USE_POSTGRESQL
    EXPECT_NE(dynamic_cast<PostgreSQLConnection*>(postgres.get()), nullptr);
#endif

#ifdef USE_SQLITE
    EXPECT_NE(dynamic_cast<SQLiteConnection*>(sqlite.get()), nullptr);
#endif

#ifdef USE_MONGODB
    auto mongo = createDBConnection(mongoConfig);
    EXPECT_NE(dynamic_cast<MongoDBConnection*>(mongo.get()), nullptr);
#endif
}

// Test invalid database type
TEST_F(DBConnectionTest, FactoryThrowsOnInvalidType) {
    DatabaseConfig invalidConfig;
    invalidConfig.type = "invalid_db_type";
    EXPECT_THROW(createDBConnection(invalidConfig), std::runtime_error);
}

#ifdef USE_MYSQL
TEST_F(DBConnectionTest, MySQLConnectionLifecycle) {
    auto conn = std::make_unique<MySQLConnection>();
    
    // Test connection (expect failure with test credentials)
    try {
        conn->connect(mysqlConfig);
        FAIL() << "Expected connection to fail with test credentials";
    } catch (const std::runtime_error& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("Access denied for user"));
    }

    // Test disconnection (should work even if not connected)
    EXPECT_NO_THROW({
        bool result = conn->disconnect();
        EXPECT_TRUE(result);
    });
}
#endif

#ifdef USE_POSTGRESQL
TEST_F(DBConnectionTest, PostgreSQLConnectionLifecycle) {
    auto conn = std::make_unique<PostgreSQLConnection>();
    
    // Test connection (expect failure with test credentials)
    try {
        conn->connect(postgresConfig);
        FAIL() << "Expected connection to fail with test credentials";
    } catch (const std::runtime_error& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("role \"test_user\" does not exist"));
    }

    // Test disconnection (should work even if not connected)
    EXPECT_NO_THROW({
        bool result = conn->disconnect();
        EXPECT_TRUE(result);
    });
}
#endif

#ifdef USE_SQLITE
TEST_F(DBConnectionTest, SQLiteConnectionLifecycle) {
    auto conn = std::make_unique<SQLiteConnection>();
    
    // Test connection
    EXPECT_NO_THROW({
        bool result = conn->connect(sqliteConfig);
        EXPECT_TRUE(result);
    });

    // Test disconnection
    EXPECT_NO_THROW({
        bool result = conn->disconnect();
        EXPECT_TRUE(result);
    });
}
#endif

// Test backup and restore operations
class DBBackupTest : public DBConnectionTest {
protected:
    const std::string testBackupPath = "/tmp/test_backup";
};

#ifdef USE_MYSQL
TEST_F(DBBackupTest, MySQLBackupRestore) {
    auto conn = std::make_unique<MySQLConnection>();
    
    // Test backup without connection
    EXPECT_THROW(conn->createBackup(testBackupPath), std::runtime_error);
    EXPECT_THROW(conn->restoreBackup(testBackupPath), std::runtime_error);
}
#endif

#ifdef USE_POSTGRESQL
TEST_F(DBBackupTest, PostgreSQLBackupRestore) {
    auto conn = std::make_unique<PostgreSQLConnection>();
    
    // Test backup without connection
    EXPECT_THROW(conn->createBackup(testBackupPath), std::runtime_error);
    EXPECT_THROW(conn->restoreBackup(testBackupPath), std::runtime_error);
}
#endif 