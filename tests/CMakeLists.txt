# tests/CMakeLists.txt

# Try to find Google Test
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "Google Test not found. Tests will not be built.")
    return()
endif()

set(TEST_SOURCES
    test_config.cpp
    test_backup_manager.cpp
    test_restore_manager.cpp
    test_cli.cpp
    test_scheduling.cpp
)

add_executable(my_db_backup_cli_tests ${TEST_SOURCES})

# Link your production code libraries or objects
# For example, if the main project was built as a library "my_db_backup_cli_lib"
# you can link it here. Otherwise, link your .cpp files directly, or rely on the object library.

target_link_libraries(my_db_backup_cli_tests
    PRIVATE
        GTest::GTest
        GTest::Main
        pthread            # On Linux
        db_backup_lib
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        # other libraries your code depends on
)

# If you have an object library containing your src/ code, do something like:
# target_link_libraries(my_db_backup_cli_tests PRIVATE my_db_backup_cli_objects)

# For each test target, register with CTest so "ctest" can find and run them
include(GoogleTest)
gtest_discover_tests(my_db_backup_cli_tests)
