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
include Placement/CMakeFiles/Placement.dir/depend.make

# Include the progress variables for this target.
include Placement/CMakeFiles/Placement.dir/progress.make

# Include the compile flags for this target's objects.
include Placement/CMakeFiles/Placement.dir/flags.make

Placement/CMakeFiles/Placement.dir/Placement.cpp.o: Placement/CMakeFiles/Placement.dir/flags.make
Placement/CMakeFiles/Placement.dir/Placement.cpp.o: Placement/Placement.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/r04943179/Physical-Design/pa3/legalization/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Placement/CMakeFiles/Placement.dir/Placement.cpp.o"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Placement && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Placement.dir/Placement.cpp.o -c /home/r04943179/Physical-Design/pa3/legalization/src/Placement/Placement.cpp

Placement/CMakeFiles/Placement.dir/Placement.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Placement.dir/Placement.cpp.i"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Placement && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/r04943179/Physical-Design/pa3/legalization/src/Placement/Placement.cpp > CMakeFiles/Placement.dir/Placement.cpp.i

Placement/CMakeFiles/Placement.dir/Placement.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Placement.dir/Placement.cpp.s"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Placement && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/r04943179/Physical-Design/pa3/legalization/src/Placement/Placement.cpp -o CMakeFiles/Placement.dir/Placement.cpp.s

Placement/CMakeFiles/Placement.dir/Placement.cpp.o.requires:
.PHONY : Placement/CMakeFiles/Placement.dir/Placement.cpp.o.requires

Placement/CMakeFiles/Placement.dir/Placement.cpp.o.provides: Placement/CMakeFiles/Placement.dir/Placement.cpp.o.requires
	$(MAKE) -f Placement/CMakeFiles/Placement.dir/build.make Placement/CMakeFiles/Placement.dir/Placement.cpp.o.provides.build
.PHONY : Placement/CMakeFiles/Placement.dir/Placement.cpp.o.provides

Placement/CMakeFiles/Placement.dir/Placement.cpp.o.provides.build: Placement/CMakeFiles/Placement.dir/Placement.cpp.o

# Object files for target Placement
Placement_OBJECTS = \
"CMakeFiles/Placement.dir/Placement.cpp.o"

# External object files for target Placement
Placement_EXTERNAL_OBJECTS =

Placement/libPlacement.a: Placement/CMakeFiles/Placement.dir/Placement.cpp.o
Placement/libPlacement.a: Placement/CMakeFiles/Placement.dir/build.make
Placement/libPlacement.a: Placement/CMakeFiles/Placement.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libPlacement.a"
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Placement && $(CMAKE_COMMAND) -P CMakeFiles/Placement.dir/cmake_clean_target.cmake
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Placement && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Placement.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Placement/CMakeFiles/Placement.dir/build: Placement/libPlacement.a
.PHONY : Placement/CMakeFiles/Placement.dir/build

Placement/CMakeFiles/Placement.dir/requires: Placement/CMakeFiles/Placement.dir/Placement.cpp.o.requires
.PHONY : Placement/CMakeFiles/Placement.dir/requires

Placement/CMakeFiles/Placement.dir/clean:
	cd /home/r04943179/Physical-Design/pa3/legalization/src/Placement && $(CMAKE_COMMAND) -P CMakeFiles/Placement.dir/cmake_clean.cmake
.PHONY : Placement/CMakeFiles/Placement.dir/clean

Placement/CMakeFiles/Placement.dir/depend:
	cd /home/r04943179/Physical-Design/pa3/legalization/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/r04943179/Physical-Design/pa3/legalization/src /home/r04943179/Physical-Design/pa3/legalization/src/Placement /home/r04943179/Physical-Design/pa3/legalization/src /home/r04943179/Physical-Design/pa3/legalization/src/Placement /home/r04943179/Physical-Design/pa3/legalization/src/Placement/CMakeFiles/Placement.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Placement/CMakeFiles/Placement.dir/depend

