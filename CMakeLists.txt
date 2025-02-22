cmake_minimum_required(VERSION 3.10)
project(database_backup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add our custom CMake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Enable database support
add_definitions(-DUSE_POSTGRESQL)
add_definitions(-DUSE_MYSQL)
add_definitions(-DUSE_SQLITE)

# Find required packages
find_package(PostgreSQL REQUIRED)
find_package(PQXX REQUIRED)
find_package(MySQL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PostgreSQL_INCLUDE_DIRS})
include_directories(${PQXX_INCLUDE_DIR})
include_directories(${MySQL_INCLUDE_DIRS})
include_directories(${SQLite3_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

# Define source files
set(SOURCES
    src/main.cpp
    src/backup_manager.cpp
    src/config.cpp
    src/db_connection.cpp
    src/compression.cpp
    src/storage.cpp
    src/logging.cpp
    src/notifications.cpp
    src/restore_manager.cpp
    src/cli.cpp
    src/scheduling.cpp
    src/db/postgresql_connection.cpp
    src/db/mysql_connection.cpp
    src/db/sqlite_connection.cpp
    src/error/ErrorUtils.cpp
)

# Create executable
add_executable(database-backup ${SOURCES})

# Link libraries
target_link_libraries(database-backup
    PRIVATE
    ${PostgreSQL_LIBRARIES}
    PQXX::PQXX
    MySQL::MySQL
    SQLite::SQLite3
    ZLIB::ZLIB
    OpenSSL::SSL
    OpenSSL::Crypto
    fmt::fmt
    spdlog::spdlog
)

# Installation rules
install(TARGETS database-backup
    RUNTIME DESTINATION bin
)

install(FILES config.json
    DESTINATION etc/database_backup
    RENAME config.template.json
)
