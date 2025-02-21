#include "db_connection.hpp"
#include "db/mysql_connection.hpp"
#include "db/postgresql_connection.hpp"
#include "db/mongodb_connection.hpp"
#include "db/sqlite_connection.hpp"
#include <iostream>

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
    if (dbConfig.type == "mysql") {
#ifdef USE_MYSQL
        return std::make_unique<MySQLConnection>();
#else
        std::cerr << "[Warning] MySQL not compiled in. Using generic fallback.\n";
#endif
    }
    else if (dbConfig.type == "postgres" || dbConfig.type == "postgresql") {
#ifdef USE_POSTGRESQL
        return std::make_unique<PostgreSQLConnection>();
#else
        std::cerr << "[Warning] PostgreSQL not compiled in. Using generic fallback.\n";
#endif
    }
    else if (dbConfig.type == "mongodb") {
#ifdef USE_MONGODB
        return std::make_unique<MongoDBConnection>();
#else
        std::cerr << "[Warning] MongoDB not compiled in. Using generic fallback.\n";
#endif
    }
    else if (dbConfig.type == "sqlite") {
#ifdef USE_SQLITE
        return std::make_unique<SQLiteConnection>();
#else
        std::cerr << "[Warning] SQLite not compiled in. Using generic fallback.\n";
#endif
    }

    throw std::runtime_error("Unsupported database type: " + dbConfig.type);
}

// ================ Default Implementation =================
bool DBConnection::connect(const DatabaseConfig& dbConfig) {
    throw std::runtime_error("DBConnection::connect not implemented");
}

bool DBConnection::disconnect() {
    throw std::runtime_error("DBConnection::disconnect not implemented");
}

bool DBConnection::createBackup(const std::string& backupPath) {
    throw std::runtime_error("DBConnection::createBackup not implemented");
}

bool DBConnection::restoreBackup(const std::string& backupPath) {
    throw std::runtime_error("DBConnection::restoreBackup not implemented");
}
