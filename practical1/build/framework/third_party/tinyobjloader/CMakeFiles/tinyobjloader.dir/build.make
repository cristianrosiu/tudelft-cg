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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.22.2/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.22.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build

# Include any dependencies generated for this target.
include framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/compiler_depend.make

# Include the progress variables for this target.
include framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/progress.make

# Include the compile flags for this target's objects.
include framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/flags.make

framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o: framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/flags.make
framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o: ../framework/third_party/tinyobjloader/src/tiny_obj_loader.cc
framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o: framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o"
	cd /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o -MF CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o.d -o CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o -c /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/framework/third_party/tinyobjloader/src/tiny_obj_loader.cc

framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.i"
	cd /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/framework/third_party/tinyobjloader/src/tiny_obj_loader.cc > CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.i

framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.s"
	cd /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/framework/third_party/tinyobjloader/src/tiny_obj_loader.cc -o CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.s

# Object files for target tinyobjloader
tinyobjloader_OBJECTS = \
"CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o"

# External object files for target tinyobjloader
tinyobjloader_EXTERNAL_OBJECTS =

framework/third_party/tinyobjloader/libtinyobjloader.a: framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/src/tiny_obj_loader.cc.o
framework/third_party/tinyobjloader/libtinyobjloader.a: framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/build.make
framework/third_party/tinyobjloader/libtinyobjloader.a: framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libtinyobjloader.a"
	cd /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader && $(CMAKE_COMMAND) -P CMakeFiles/tinyobjloader.dir/cmake_clean_target.cmake
	cd /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tinyobjloader.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/build: framework/third_party/tinyobjloader/libtinyobjloader.a
.PHONY : framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/build

framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/clean:
	cd /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader && $(CMAKE_COMMAND) -P CMakeFiles/tinyobjloader.dir/cmake_clean.cmake
.PHONY : framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/clean

framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/depend:
	cd /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1 /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/framework/third_party/tinyobjloader /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader /Users/cristianrosiu/Documents/TUDelft/tudelft-cg/practical1/build/framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : framework/third_party/tinyobjloader/CMakeFiles/tinyobjloader.dir/depend

