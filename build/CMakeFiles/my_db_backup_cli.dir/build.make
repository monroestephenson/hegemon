# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/monroestephenson/database_backup

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/monroestephenson/database_backup/build

# Include any dependencies generated for this target.
include CMakeFiles/my_db_backup_cli.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/my_db_backup_cli.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/my_db_backup_cli.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/my_db_backup_cli.dir/flags.make

CMakeFiles/my_db_backup_cli.dir/codegen:
.PHONY : CMakeFiles/my_db_backup_cli.dir/codegen

CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o: CMakeFiles/my_db_backup_cli.dir/flags.make
CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o: /Users/monroestephenson/database_backup/src/main.cpp
CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o: CMakeFiles/my_db_backup_cli.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/monroestephenson/database_backup/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o -MF CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o.d -o CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o -c /Users/monroestephenson/database_backup/src/main.cpp

CMakeFiles/my_db_backup_cli.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/my_db_backup_cli.dir/src/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/monroestephenson/database_backup/src/main.cpp > CMakeFiles/my_db_backup_cli.dir/src/main.cpp.i

CMakeFiles/my_db_backup_cli.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/my_db_backup_cli.dir/src/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/monroestephenson/database_backup/src/main.cpp -o CMakeFiles/my_db_backup_cli.dir/src/main.cpp.s

CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o: CMakeFiles/my_db_backup_cli.dir/flags.make
CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o: /Users/monroestephenson/database_backup/src/cli.cpp
CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o: CMakeFiles/my_db_backup_cli.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/monroestephenson/database_backup/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o -MF CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o.d -o CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o -c /Users/monroestephenson/database_backup/src/cli.cpp

CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/monroestephenson/database_backup/src/cli.cpp > CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.i

CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/monroestephenson/database_backup/src/cli.cpp -o CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.s

CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o: CMakeFiles/my_db_backup_cli.dir/flags.make
CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o: /Users/monroestephenson/database_backup/src/scheduling.cpp
CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o: CMakeFiles/my_db_backup_cli.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/monroestephenson/database_backup/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o -MF CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o.d -o CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o -c /Users/monroestephenson/database_backup/src/scheduling.cpp

CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/monroestephenson/database_backup/src/scheduling.cpp > CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.i

CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/monroestephenson/database_backup/src/scheduling.cpp -o CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.s

# Object files for target my_db_backup_cli
my_db_backup_cli_OBJECTS = \
"CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o" \
"CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o" \
"CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o"

# External object files for target my_db_backup_cli
my_db_backup_cli_EXTERNAL_OBJECTS =

my_db_backup_cli: CMakeFiles/my_db_backup_cli.dir/src/main.cpp.o
my_db_backup_cli: CMakeFiles/my_db_backup_cli.dir/src/cli.cpp.o
my_db_backup_cli: CMakeFiles/my_db_backup_cli.dir/src/scheduling.cpp.o
my_db_backup_cli: CMakeFiles/my_db_backup_cli.dir/build.make
my_db_backup_cli: libdb_backup_lib.a
my_db_backup_cli: /usr/local/lib/libspdlog.1.15.1.dylib
my_db_backup_cli: /usr/local/lib/libfmt.11.1.3.dylib
my_db_backup_cli: /usr/local/lib/libmysqlclient.dylib
my_db_backup_cli: /usr/local/lib/postgresql@14/libpq.dylib
my_db_backup_cli: /usr/local/lib/libpqxx.dylib
my_db_backup_cli: /usr/local/lib/libmongocxx.4.0.0.dylib
my_db_backup_cli: /usr/local/lib/libbsoncxx.4.0.0.dylib
my_db_backup_cli: /Library/Developer/CommandLineTools/SDKs/MacOSX15.2.sdk/usr/lib/libsqlite3.tbd
my_db_backup_cli: CMakeFiles/my_db_backup_cli.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/monroestephenson/database_backup/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable my_db_backup_cli"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/my_db_backup_cli.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/my_db_backup_cli.dir/build: my_db_backup_cli
.PHONY : CMakeFiles/my_db_backup_cli.dir/build

CMakeFiles/my_db_backup_cli.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/my_db_backup_cli.dir/cmake_clean.cmake
.PHONY : CMakeFiles/my_db_backup_cli.dir/clean

CMakeFiles/my_db_backup_cli.dir/depend:
	cd /Users/monroestephenson/database_backup/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/monroestephenson/database_backup /Users/monroestephenson/database_backup /Users/monroestephenson/database_backup/build /Users/monroestephenson/database_backup/build /Users/monroestephenson/database_backup/build/CMakeFiles/my_db_backup_cli.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/my_db_backup_cli.dir/depend

