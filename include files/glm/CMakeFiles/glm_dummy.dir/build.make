# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2

# Include any dependencies generated for this target.
include glm/CMakeFiles/glm_dummy.dir/depend.make

# Include the progress variables for this target.
include glm/CMakeFiles/glm_dummy.dir/progress.make

# Include the compile flags for this target's objects.
include glm/CMakeFiles/glm_dummy.dir/flags.make

glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o: glm/CMakeFiles/glm_dummy.dir/flags.make
glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o: glm/detail/dummy.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o"
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o -c /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm/detail/dummy.cpp

glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/glm_dummy.dir/detail/dummy.cpp.i"
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm/detail/dummy.cpp > CMakeFiles/glm_dummy.dir/detail/dummy.cpp.i

glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/glm_dummy.dir/detail/dummy.cpp.s"
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm/detail/dummy.cpp -o CMakeFiles/glm_dummy.dir/detail/dummy.cpp.s

glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.requires:

.PHONY : glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.requires

glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.provides: glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.requires
	$(MAKE) -f glm/CMakeFiles/glm_dummy.dir/build.make glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.provides.build
.PHONY : glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.provides

glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.provides.build: glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o


glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o: glm/CMakeFiles/glm_dummy.dir/flags.make
glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o: glm/detail/glm.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o"
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/glm_dummy.dir/detail/glm.cpp.o -c /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm/detail/glm.cpp

glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/glm_dummy.dir/detail/glm.cpp.i"
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm/detail/glm.cpp > CMakeFiles/glm_dummy.dir/detail/glm.cpp.i

glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/glm_dummy.dir/detail/glm.cpp.s"
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm/detail/glm.cpp -o CMakeFiles/glm_dummy.dir/detail/glm.cpp.s

glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.requires:

.PHONY : glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.requires

glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.provides: glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.requires
	$(MAKE) -f glm/CMakeFiles/glm_dummy.dir/build.make glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.provides.build
.PHONY : glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.provides

glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.provides.build: glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o


# Object files for target glm_dummy
glm_dummy_OBJECTS = \
"CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o" \
"CMakeFiles/glm_dummy.dir/detail/glm.cpp.o"

# External object files for target glm_dummy
glm_dummy_EXTERNAL_OBJECTS =

glm/glm_dummy: glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o
glm/glm_dummy: glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o
glm/glm_dummy: glm/CMakeFiles/glm_dummy.dir/build.make
glm/glm_dummy: glm/CMakeFiles/glm_dummy.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable glm_dummy"
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/glm_dummy.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
glm/CMakeFiles/glm_dummy.dir/build: glm/glm_dummy

.PHONY : glm/CMakeFiles/glm_dummy.dir/build

glm/CMakeFiles/glm_dummy.dir/requires: glm/CMakeFiles/glm_dummy.dir/detail/dummy.cpp.o.requires
glm/CMakeFiles/glm_dummy.dir/requires: glm/CMakeFiles/glm_dummy.dir/detail/glm.cpp.o.requires

.PHONY : glm/CMakeFiles/glm_dummy.dir/requires

glm/CMakeFiles/glm_dummy.dir/clean:
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm && $(CMAKE_COMMAND) -P CMakeFiles/glm_dummy.dir/cmake_clean.cmake
.PHONY : glm/CMakeFiles/glm_dummy.dir/clean

glm/CMakeFiles/glm_dummy.dir/depend:
	cd /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2 /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2 /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm /home/iiit/Downloads/glm_0.9.9_a2.orig/glm-0.9.9-a2/glm/CMakeFiles/glm_dummy.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : glm/CMakeFiles/glm_dummy.dir/depend

