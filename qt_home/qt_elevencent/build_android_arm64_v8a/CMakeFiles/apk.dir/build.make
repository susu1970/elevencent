# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_SOURCE_DIR = /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent/build_android_arm64_v8a

# Utility rule file for apk.

# Include any custom commands dependencies for this target.
include CMakeFiles/apk.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/apk.dir/progress.make

apk: CMakeFiles/apk.dir/build.make
.PHONY : apk

# Rule to build all files generated by this target.
CMakeFiles/apk.dir/build: apk
.PHONY : CMakeFiles/apk.dir/build

CMakeFiles/apk.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/apk.dir/cmake_clean.cmake
.PHONY : CMakeFiles/apk.dir/clean

CMakeFiles/apk.dir/depend:
	cd /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent/build_android_arm64_v8a && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent/build_android_arm64_v8a /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent/build_android_arm64_v8a /dc/remote/susu1970/node2/static/document/code/me/elevencent/qt_home/qt_elevencent/build_android_arm64_v8a/CMakeFiles/apk.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/apk.dir/depend
