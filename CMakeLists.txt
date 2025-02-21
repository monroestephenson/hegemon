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
    find_package(MySQL)
    if(MySQL_FOUND)
        add_definitions(-DUSE_MYSQL)
        include_directories(${MYSQL_INCLUDE_DIR})
    endif()
endif()

if(USE_POSTGRESQL)
    find_package(PostgreSQL)
    find_package(PQXX)
    if(PostgreSQL_FOUND AND PQXX_FOUND)
        add_definitions(-DUSE_POSTGRESQL)
        include_directories(${PostgreSQL_INCLUDE_DIRS} ${PQXX_INCLUDE_DIR})
    endif()
endif()

if(USE_MONGODB)
    find_package(mongocxx)
    find_package(bsoncxx)
    if(MONGOCXX_FOUND AND BSONCXX_FOUND)
        add_definitions(-DUSE_MONGODB)
    endif()
endif()

if(USE_SQLITE)
    find_package(SQLite3)
    if(SQLite3_FOUND)
        add_definitions(-DUSE_SQLITE)
    endif()
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

# For macOS, explicitly add system include paths
if(APPLE)
    include_directories(SYSTEM /Library/Developer/CommandLineTools/usr/include/c++/v1)
    include_directories(SYSTEM /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include)
endif()

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
if(MySQL_FOUND)
    target_include_directories(db_backup_lib PRIVATE ${MYSQL_INCLUDE_DIR})
    target_link_libraries(db_backup_lib PRIVATE ${MYSQL_LIBRARIES})
endif()

if(PostgreSQL_FOUND AND PQXX_FOUND)
    target_include_directories(db_backup_lib PRIVATE ${PostgreSQL_INCLUDE_DIRS} ${PQXX_INCLUDE_DIR})
    target_link_libraries(db_backup_lib PRIVATE ${PostgreSQL_LIBRARIES} ${PQXX_LIBRARIES})
endif()

if(MONGOCXX_FOUND AND BSONCXX_FOUND)
    target_link_libraries(db_backup_lib PRIVATE 
        mongo::mongocxx_shared
        mongo::bsoncxx_shared
    )
endif()

if(SQLite3_FOUND)
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

# Installation rules
install(TARGETS my_db_backup_cli
    RUNTIME DESTINATION bin
)

install(FILES config.json
    DESTINATION etc/database_backup
    RENAME config.template.json
)

install(FILES README.md LICENSE
    DESTINATION share/doc/database_backup
)
