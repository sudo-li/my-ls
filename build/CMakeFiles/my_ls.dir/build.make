# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/li/my_ls

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/li/my_ls/build

# Include any dependencies generated for this target.
include CMakeFiles/my_ls.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/my_ls.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/my_ls.dir/flags.make

CMakeFiles/my_ls.dir/my_ls.c.o: CMakeFiles/my_ls.dir/flags.make
CMakeFiles/my_ls.dir/my_ls.c.o: ../my_ls.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/li/my_ls/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/my_ls.dir/my_ls.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/my_ls.dir/my_ls.c.o   -c /home/li/my_ls/my_ls.c

CMakeFiles/my_ls.dir/my_ls.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/my_ls.dir/my_ls.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/li/my_ls/my_ls.c > CMakeFiles/my_ls.dir/my_ls.c.i

CMakeFiles/my_ls.dir/my_ls.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/my_ls.dir/my_ls.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/li/my_ls/my_ls.c -o CMakeFiles/my_ls.dir/my_ls.c.s

# Object files for target my_ls
my_ls_OBJECTS = \
"CMakeFiles/my_ls.dir/my_ls.c.o"

# External object files for target my_ls
my_ls_EXTERNAL_OBJECTS =

../bin/my_ls: CMakeFiles/my_ls.dir/my_ls.c.o
../bin/my_ls: CMakeFiles/my_ls.dir/build.make
../bin/my_ls: CMakeFiles/my_ls.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/li/my_ls/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../bin/my_ls"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/my_ls.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/my_ls.dir/build: ../bin/my_ls

.PHONY : CMakeFiles/my_ls.dir/build

CMakeFiles/my_ls.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/my_ls.dir/cmake_clean.cmake
.PHONY : CMakeFiles/my_ls.dir/clean

CMakeFiles/my_ls.dir/depend:
	cd /home/li/my_ls/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/li/my_ls /home/li/my_ls /home/li/my_ls/build /home/li/my_ls/build /home/li/my_ls/build/CMakeFiles/my_ls.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/my_ls.dir/depend
