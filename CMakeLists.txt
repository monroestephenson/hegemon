cmake_minimum_required(VERSION 3.10)
project(my_db_backup_cli)

# Add our custom CMake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT ${MACOS_SDK_PATH})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot ${MACOS_SDK_PATH}")
endif()

# Add compiler flags for warnings and debugging
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -g)
endif()

# Find required packages
find_package(spdlog REQUIRED)
find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)

# Database dependencies
option(USE_MYSQL "Enable MySQL support" ON)
option(USE_POSTGRESQL "Enable PostgreSQL support" ON)
option(USE_MONGODB "Enable MongoDB support" OFF)
option(USE_SQLITE "Enable SQLite support" ON)

if(USE_MYSQL)
    find_package(MySQL REQUIRED)
    add_definitions(-DUSE_MYSQL)
    include_directories(${MYSQL_INCLUDE_DIR})
endif()

if(USE_POSTGRESQL)
    find_package(PostgreSQL REQUIRED)
    find_package(PQXX REQUIRED)
    add_definitions(-DUSE_POSTGRESQL)
    include_directories(${PostgreSQL_INCLUDE_DIRS} ${PQXX_INCLUDE_DIR})
endif()

if(USE_MONGODB)
    find_package(mongocxx REQUIRED)
    find_package(bsoncxx REQUIRED)
    add_definitions(-DUSE_MONGODB)
endif()

if(USE_SQLITE)
    find_package(SQLite3 REQUIRED)
    add_definitions(-DUSE_SQLITE)
endif()

# Example: If you have the libraries installed system-wide, you might do:
# find_package(CLI11 REQUIRED)
# find_package(nlohmann_json REQUIRED)
# find_package(ZLIB REQUIRED)
# find_package(CURL REQUIRED)
# find_package(MySQLConnectorCXX REQUIRED)
# find_package(PostgreSQL REQUIRED)
# find_package(LibMongoCXX REQUIRED)
# etc...

# For demonstration, we'll just include some definitions. 
# In a real scenario, be sure to link them properly:
# target_link_libraries(my_db_backup_cli PRIVATE CLI11::CLI11 nlohmann_json::nlohmann_json spdlog::spdlog ...)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

# For macOS, explicitly add system include paths
if(APPLE)
    include_directories(SYSTEM /Library/Developer/CommandLineTools/usr/include/c++/v1)
    include_directories(SYSTEM /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include)
endif()

# Add Google Test
add_subdirectory(external/googletest)
enable_testing()

# Define source files for the library
set(LIB_SOURCES
    src/backup_manager.cpp
    src/config.cpp
    src/db_connection.cpp
    src/compression.cpp
    src/storage.cpp
    src/logging.cpp
    src/notifications.cpp
    src/restore_manager.cpp
    src/db/mysql_connection.cpp
    src/db/postgresql_connection.cpp
    src/db/mongodb_connection.cpp
    src/db/sqlite_connection.cpp
    src/error/ErrorUtils.cpp
)

# Create a library from the common sources
add_library(db_backup_lib STATIC ${LIB_SOURCES})
target_link_libraries(db_backup_lib PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Add database-specific libraries
if(USE_MYSQL)
    target_include_directories(db_backup_lib PRIVATE ${MYSQL_INCLUDE_DIR})
    target_link_libraries(db_backup_lib PRIVATE ${MYSQL_LIBRARIES})
endif()

if(USE_POSTGRESQL)
    target_include_directories(db_backup_lib PRIVATE ${PostgreSQL_INCLUDE_DIRS} ${PQXX_INCLUDE_DIR})
    target_link_libraries(db_backup_lib PRIVATE ${PostgreSQL_LIBRARIES} ${PQXX_LIBRARIES})
endif()

if(USE_MONGODB)
    target_link_libraries(db_backup_lib PRIVATE 
        mongo::mongocxx_shared
        mongo::bsoncxx_shared
    )
endif()

if(USE_SQLITE)
    target_link_libraries(db_backup_lib PRIVATE SQLite::SQLite3)
endif()

# Main executable
add_executable(my_db_backup_cli
    src/main.cpp
    src/cli.cpp
    src/scheduling.cpp
)

target_link_libraries(my_db_backup_cli PRIVATE 
    db_backup_lib
    spdlog::spdlog
    CLI11::CLI11
    nlohmann_json::nlohmann_json
)

# Tests
add_executable(unit_tests
    tests/test_backup_manager.cpp
    tests/db/test_db_connections.cpp
)

target_include_directories(unit_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/mocks
)

target_link_libraries(unit_tests PRIVATE
    db_backup_lib
    gtest
    gtest_main
    gmock
    gmock_main
)

add_test(NAME unit_tests COMMAND unit_tests)
