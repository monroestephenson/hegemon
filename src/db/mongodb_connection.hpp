#pragma once

#include "../db_connection.hpp"
#include <memory>

// Forward declarations
#ifdef USE_MONGODB
namespace mongocxx {
inline namespace v_noabi {
    class client;
    class instance;
}
}
#endif

class MongoDBConnection : public IDBConnection {
public:
    MongoDBConnection();
    ~MongoDBConnection() override;

    bool connect(const DatabaseConfig& dbConfig) override;
    bool disconnect() override;
    bool createBackup(const std::string& backupPath) override;
    bool restoreBackup(const std::string& backupPath) override;

private:
#ifdef USE_MONGODB
    std::unique_ptr<mongocxx::v_noabi::instance> instance;
    std::unique_ptr<mongocxx::v_noabi::client> client;
#endif
    bool connected;
    std::string currentDbName;
}; 