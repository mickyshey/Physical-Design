# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/r04943179/Physical-Design/pa3/legalization/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/r04943179/Physical-Design/pa3/legalization/src

# Include any dependencies generated for this target.
include Parser/CMakeFiles/Parser.dir/depend.make

# Include the progress variables for this target.
include Parser/CMakeFiles/Parser.dir/progress.make

# Include the compile flags for this target's objects.
include Parser/CMakeFiles/Parser.dir/flags.make

Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o: Parser/CMakeFiles/Parser.dir/flags.make
Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o: Parser/BookshelfParser.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/r04943179/Physical-Design/pa3/legalization/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Parser && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Parser.dir/BookshelfParser.cpp.o -c /home/r04943179/Physical-Design/pa3/legalization/src/Parser/BookshelfParser.cpp

Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Parser.dir/BookshelfParser.cpp.i"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Parser && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/r04943179/Physical-Design/pa3/legalization/src/Parser/BookshelfParser.cpp > CMakeFiles/Parser.dir/BookshelfParser.cpp.i

Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Parser.dir/BookshelfParser.cpp.s"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Parser && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/r04943179/Physical-Design/pa3/legalization/src/Parser/BookshelfParser.cpp -o CMakeFiles/Parser.dir/BookshelfParser.cpp.s

Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.requires:
.PHONY : Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.requires

Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.provides: Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.requires
	$(MAKE) -f Parser/CMakeFiles/Parser.dir/build.make Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.provides.build
.PHONY : Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.provides

Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.provides.build: Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o

# Object files for target Parser
Parser_OBJECTS = \
"CMakeFiles/Parser.dir/BookshelfParser.cpp.o"

# External object files for target Parser
Parser_EXTERNAL_OBJECTS =

Parser/libParser.a: Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o
Parser/libParser.a: Parser/CMakeFiles/Parser.dir/build.make
Parser/libParser.a: Parser/CMakeFiles/Parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libParser.a"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Parser && $(CMAKE_COMMAND) -P CMakeFiles/Parser.dir/cmake_clean_target.cmake
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Parser && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Parser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Parser/CMakeFiles/Parser.dir/build: Parser/libParser.a
.PHONY : Parser/CMakeFiles/Parser.dir/build

Parser/CMakeFiles/Parser.dir/requires: Parser/CMakeFiles/Parser.dir/BookshelfParser.cpp.o.requires
.PHONY : Parser/CMakeFiles/Parser.dir/requires

Parser/CMakeFiles/Parser.dir/clean:
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Parser && $(CMAKE_COMMAND) -P CMakeFiles/Parser.dir/cmake_clean.cmake
.PHONY : Parser/CMakeFiles/Parser.dir/clean

Parser/CMakeFiles/Parser.dir/depend:
	cd /home/r04943179/Physical-Design/pa3/legalization/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/r04943179/Physical-Design/pa3/legalization/src /home/r04943179/Physical-Design/pa3/legalization/src/Parser /home/r04943179/Physical-Design/pa3/legalization/src /home/r04943179/Physical-Design/pa3/legalization/src/Parser /home/r04943179/Physical-Design/pa3/legalization/src/Parser/CMakeFiles/Parser.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Parser/CMakeFiles/Parser.dir/depend

