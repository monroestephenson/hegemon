cmake_minimum_required(VERSION 3.10)
project(my_db_backup_cli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Example: If you have the libraries installed system-wide, you might do:
# find_package(CLI11 REQUIRED)
# find_package(nlohmann_json REQUIRED)
# find_package(spdlog REQUIRED)
# find_package(ZLIB REQUIRED)
# find_package(CURL REQUIRED)
# find_package(MySQLConnectorCXX REQUIRED)
# find_package(PostgreSQL REQUIRED)
# find_package(LibMongoCXX REQUIRED)
# etc...

# For demonstration, we’ll just include some definitions. 
# In a real scenario, be sure to link them properly:
# target_link_libraries(my_db_backup_cli PRIVATE CLI11::CLI11 nlohmann_json::nlohmann_json spdlog::spdlog ...)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(my_db_backup_cli
    src/main.cpp
    src/cli.cpp
    src/config.cpp
    src/db_connection.cpp
    src/backup_manager.cpp
    src/restore_manager.cpp
    src/compression.cpp
    src/storage.cpp
    src/logging.cpp
    src/notifications.cpp
    src/scheduling.cpp
)

# If libraries are found, link them (example):
# target_link_libraries(my_db_backup_cli PRIVATE CLI11::CLI11 nlohmann_json::nlohmann_json ...)
cmake_minimum_required(VERSION 3.10)
project(my_db_backup_cli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# (Your existing code for building main project)
# add_executable, link libraries, etc...

# Optionally add googletest if it's part of the repo as a submodule
add_subdirectory(external/googletest)

# Add your tests
enable_testing()
add_subdirectory(tests)
