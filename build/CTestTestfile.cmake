# CMake generated Testfile for 
# Source directory: /Users/monroestephenson/database_backup
# Build directory: /Users/monroestephenson/database_backup/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(unit_tests "/Users/monroestephenson/database_backup/build/unit_tests")
set_tests_properties(unit_tests PROPERTIES  _BACKTRACE_TRIPLES "/Users/monroestephenson/database_backup/CMakeLists.txt;167;add_test;/Users/monroestephenson/database_backup/CMakeLists.txt;0;")
subdirs("external/googletest")
