# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/susu1970/susu1970/work/elevencent

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/susu1970/susu1970/work/elevencent/build

# Include any dependencies generated for this target.
include bin_test/CMakeFiles/global.dir/depend.make

# Include the progress variables for this target.
include bin_test/CMakeFiles/global.dir/progress.make

# Include the compile flags for this target's objects.
include bin_test/CMakeFiles/global.dir/flags.make

bin_test/CMakeFiles/global.dir/global.cc.o: bin_test/CMakeFiles/global.dir/flags.make
bin_test/CMakeFiles/global.dir/global.cc.o: ../test/global.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/susu1970/susu1970/work/elevencent/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object bin_test/CMakeFiles/global.dir/global.cc.o"
	cd /home/susu1970/susu1970/work/elevencent/build/bin_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/global.dir/global.cc.o -c /home/susu1970/susu1970/work/elevencent/test/global.cc

bin_test/CMakeFiles/global.dir/global.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/global.dir/global.cc.i"
	cd /home/susu1970/susu1970/work/elevencent/build/bin_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/susu1970/susu1970/work/elevencent/test/global.cc > CMakeFiles/global.dir/global.cc.i

bin_test/CMakeFiles/global.dir/global.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/global.dir/global.cc.s"
	cd /home/susu1970/susu1970/work/elevencent/build/bin_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/susu1970/susu1970/work/elevencent/test/global.cc -o CMakeFiles/global.dir/global.cc.s

# Object files for target global
global_OBJECTS = \
"CMakeFiles/global.dir/global.cc.o"

# External object files for target global
global_EXTERNAL_OBJECTS =

bin_test/global: bin_test/CMakeFiles/global.dir/global.cc.o
bin_test/global: bin_test/CMakeFiles/global.dir/build.make
bin_test/global: bin_test/CMakeFiles/global.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/susu1970/susu1970/work/elevencent/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable global"
	cd /home/susu1970/susu1970/work/elevencent/build/bin_test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/global.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
bin_test/CMakeFiles/global.dir/build: bin_test/global

.PHONY : bin_test/CMakeFiles/global.dir/build

bin_test/CMakeFiles/global.dir/clean:
	cd /home/susu1970/susu1970/work/elevencent/build/bin_test && $(CMAKE_COMMAND) -P CMakeFiles/global.dir/cmake_clean.cmake
.PHONY : bin_test/CMakeFiles/global.dir/clean

bin_test/CMakeFiles/global.dir/depend:
	cd /home/susu1970/susu1970/work/elevencent/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/susu1970/susu1970/work/elevencent /home/susu1970/susu1970/work/elevencent/test /home/susu1970/susu1970/work/elevencent/build /home/susu1970/susu1970/work/elevencent/build/bin_test /home/susu1970/susu1970/work/elevencent/build/bin_test/CMakeFiles/global.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : bin_test/CMakeFiles/global.dir/depend
