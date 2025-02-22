cmake_minimum_required(VERSION 3.10)
project(hegemon)

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
add_executable(hegemon ${SOURCES})

# Link libraries
target_link_libraries(hegemon
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
install(TARGETS hegemon
    RUNTIME DESTINATION bin
)

install(FILES config.json
    DESTINATION etc/hegemon
    RENAME config.template.json
)

# Database support options
option(USE_MYSQL "Enable MySQL support" ON)
option(USE_POSTGRESQL "Enable PostgreSQL support" ON)
option(USE_SQLITE "Enable SQLite support" ON)
option(USE_MONGODB "Enable MongoDB support" OFF)

# Configure database support
if(USE_MYSQL)
    find_package(MySQL REQUIRED)
    add_definitions(-DUSE_MYSQL)
    include_directories(${MySQL_INCLUDE_DIRS})
    target_link_libraries(hegemon PRIVATE MySQL::MySQL)
endif()

if(USE_POSTGRESQL)
    find_package(PostgreSQL REQUIRED)
    add_definitions(-DUSE_POSTGRESQL)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(hegemon PRIVATE ${PostgreSQL_LIBRARIES})
endif()

if(USE_SQLITE)
    find_package(SQLite3 REQUIRED)
    add_definitions(-DUSE_SQLITE)
    include_directories(${SQLite3_INCLUDE_DIRS})
    target_link_libraries(hegemon PRIVATE SQLite::SQLite3)
endif()

if(USE_MONGODB)
    find_package(mongocxx REQUIRED)
    add_definitions(-DUSE_MONGODB)
    include_directories(${MONGOCXX_INCLUDE_DIRS})
    target_link_libraries(hegemon PRIVATE ${MONGOCXX_LIBRARIES})
endif()
