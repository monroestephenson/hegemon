#pragma once

#include "db_connection.hpp"
#include <gmock/gmock.h> // Google Mock

class MockDBConnection : public IDBConnection {
public:
    MOCK_METHOD(bool, connect, (const DatabaseConfig& dbConfig), (override));
    MOCK_METHOD(bool, disconnect, (), (override));
    MOCK_METHOD(bool, createBackup, (const std::string& backupPath), (override));
    MOCK_METHOD(bool, restoreBackup, (const std::string& backupPath), (override));
};
