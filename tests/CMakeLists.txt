# tests/CMakeLists.txt

if(BUILD_TESTS)
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
        test_compression.cpp
    )

    add_executable(database_backup_tests ${TEST_SOURCES})

    # Add MySQL support if enabled
    if(MySQL_FOUND)
        target_compile_definitions(database_backup_tests PRIVATE -DUSE_MYSQL)
        target_include_directories(database_backup_tests PRIVATE ${MYSQL_INCLUDE_DIR})
    endif()

    target_link_libraries(database_backup_tests
        PRIVATE
            GTest::GTest
            GTest::Main
            GTest::gmock
            GTest::gmock_main
            pthread            # On Linux
            db_backup_lib
            spdlog::spdlog
            nlohmann_json::nlohmann_json
    )

    # For each test target, register with CTest
    include(GoogleTest)
    gtest_discover_tests(database_backup_tests)
endif()
