# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/lai/eecs583/hw0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lai/eecs583/hw0/build

# Utility rule file for install-samplePass.

# Include any custom commands dependencies for this target.
include samplePass/CMakeFiles/install-samplePass.dir/compiler_depend.make

# Include the progress variables for this target.
include samplePass/CMakeFiles/install-samplePass.dir/progress.make

samplePass/CMakeFiles/install-samplePass:
	cd /home/lai/eecs583/hw0/build/samplePass && /usr/bin/cmake -DCMAKE_INSTALL_COMPONENT="samplePass" -P /home/lai/eecs583/hw0/build/cmake_install.cmake

install-samplePass: samplePass/CMakeFiles/install-samplePass
install-samplePass: samplePass/CMakeFiles/install-samplePass.dir/build.make
.PHONY : install-samplePass

# Rule to build all files generated by this target.
samplePass/CMakeFiles/install-samplePass.dir/build: install-samplePass
.PHONY : samplePass/CMakeFiles/install-samplePass.dir/build

samplePass/CMakeFiles/install-samplePass.dir/clean:
	cd /home/lai/eecs583/hw0/build/samplePass && $(CMAKE_COMMAND) -P CMakeFiles/install-samplePass.dir/cmake_clean.cmake
.PHONY : samplePass/CMakeFiles/install-samplePass.dir/clean

samplePass/CMakeFiles/install-samplePass.dir/depend:
	cd /home/lai/eecs583/hw0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lai/eecs583/hw0 /home/lai/eecs583/hw0/samplePass /home/lai/eecs583/hw0/build /home/lai/eecs583/hw0/build/samplePass /home/lai/eecs583/hw0/build/samplePass/CMakeFiles/install-samplePass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samplePass/CMakeFiles/install-samplePass.dir/depend

