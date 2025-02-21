#pragma once

#include "../db_connection.hpp"
#include <gmock/gmock.h>

class MockDBConnection : public IDBConnection {
public:
    MOCK_METHOD(bool, connect, (const DatabaseConfig& config), (override));
    MOCK_METHOD(bool, disconnect, (), (override));
    MOCK_METHOD(bool, createBackup, (const std::string& path), (override));
    MOCK_METHOD(bool, restoreBackup, (const std::string& path), (override));
}; 