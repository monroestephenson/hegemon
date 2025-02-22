#include "db_connection.hpp"
#include "db/mysql_connection.hpp"
#include "db/postgresql_connection.hpp"
#include "db/mongodb_connection.hpp"
#include "db/sqlite_connection.hpp"
#include "error/ErrorUtils.hpp"

using namespace dbbackup::error;

namespace dbbackup {

std::unique_ptr<IDBConnection> createDBConnection(const DatabaseConfig& dbConfig) {
    DB_TRY_CATCH_LOG("DBConnection", {
        if (dbConfig.type == "mysql") {
#ifdef USE_MYSQL
            return std::make_unique<MySQLConnection>();
#else
            DB_THROW(ConfigurationError, "MySQL support not enabled");
#endif
        } else if (dbConfig.type == "postgresql") {
#ifdef USE_POSTGRESQL
            return std::make_unique<PostgreSQLConnection>();
#else
            DB_THROW(ConfigurationError, "PostgreSQL support not enabled");
#endif
        } else if (dbConfig.type == "mongodb") {
#ifdef USE_MONGODB
            return std::make_unique<MongoDBConnection>();
#else
            DB_THROW(ConfigurationError, "MongoDB support not enabled");
#endif
        } else if (dbConfig.type == "sqlite") {
#ifdef USE_SQLITE
            return std::make_unique<SQLiteConnection>();
#else
            DB_THROW(ConfigurationError, "SQLite support not enabled");
#endif
        } else {
            DB_THROW(ConfigurationError, "Unsupported database type: " + dbConfig.type);
        }
    });
    return nullptr;
}

} // namespace dbbackup 