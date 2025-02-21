#include "db_connection.hpp"
#include "db/mysql_connection.hpp"
#include "db/postgresql_connection.hpp"
#include "db/mongodb_connection.hpp"
#include "db/sqlite_connection.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>

using namespace dbbackup::error;

// ================ MySQL Implementation Stub =================
#ifdef USE_MYSQL
#include <mysql.h>
#endif

// ================ PostgreSQL Implementation Stub =================
#ifdef USE_POSTGRESQL
#include <pqxx/pqxx>
#endif

// ================ MongoDB Implementation Stub =================
#ifdef USE_MONGODB
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#endif

// ================ SQLite Implementation Stub =================
#ifdef USE_SQLITE
#include <sqlite3.h>
#endif

// ================ Factory Function =================
std::unique_ptr<IDBConnection> createDBConnection(const DatabaseConfig& dbConfig) {
    DB_TRY_CATCH_LOG("DBConnection", {
        DB_CHECK(!dbConfig.type.empty(), ConfigurationError, "Database type cannot be empty");
        
        if (dbConfig.type == "mysql") {
#ifdef USE_MYSQL
            return std::make_unique<MySQLConnection>();
#else
            DB_THROW(ConfigurationError, "MySQL support not compiled in");
#endif
        }
        else if (dbConfig.type == "postgres" || dbConfig.type == "postgresql") {
#ifdef USE_POSTGRESQL
            return std::make_unique<PostgreSQLConnection>();
#else
            DB_THROW(ConfigurationError, "PostgreSQL support not compiled in");
#endif
        }
        else if (dbConfig.type == "mongodb") {
#ifdef USE_MONGODB
            return std::make_unique<MongoDBConnection>();
#else
            DB_THROW(ConfigurationError, "MongoDB support not compiled in");
#endif
        }
        else if (dbConfig.type == "sqlite") {
#ifdef USE_SQLITE
            return std::make_unique<SQLiteConnection>();
#else
            DB_THROW(ConfigurationError, "SQLite support not compiled in");
#endif
        }

        DB_THROW(ConfigurationError, "Unsupported database type: " + dbConfig.type);
    });
    
    // This return is never reached due to DB_TRY_CATCH_LOG, but needed for compilation
    return nullptr;
}

// ================ Default Implementation =================
bool DBConnection::connect(const DatabaseConfig& dbConfig) {
    DB_THROW(ConnectionError, "connect() not implemented for this database type");
}

bool DBConnection::disconnect() {
    DB_THROW(ConnectionError, "disconnect() not implemented for this database type");
}

bool DBConnection::createBackup(const std::string& backupPath) {
    DB_THROW(BackupError, "createBackup() not implemented for this database type");
}

bool DBConnection::restoreBackup(const std::string& backupPath) {
    DB_THROW(RestoreError, "restoreBackup() not implemented for this database type");
}
