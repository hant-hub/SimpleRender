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
CMAKE_COMMAND = /home/elijah/.local/lib/python3.8/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/elijah/.local/lib/python3.8/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/elijah/Desktop/Dev/Git/repos/SimpleRender

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/elijah/Desktop/Dev/Git/repos/SimpleRender

# Include any dependencies generated for this target.
include CMakeFiles/SimpleRender.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/SimpleRender.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/SimpleRender.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SimpleRender.dir/flags.make

CMakeFiles/SimpleRender.dir/src/command/command.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/command/command.c.o: src/command/command.c
CMakeFiles/SimpleRender.dir/src/command/command.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/SimpleRender.dir/src/command/command.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/command/command.c.o -MF CMakeFiles/SimpleRender.dir/src/command/command.c.o.d -o CMakeFiles/SimpleRender.dir/src/command/command.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/command/command.c

CMakeFiles/SimpleRender.dir/src/command/command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/command/command.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/command/command.c > CMakeFiles/SimpleRender.dir/src/command/command.c.i

CMakeFiles/SimpleRender.dir/src/command/command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/command/command.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/command/command.c -o CMakeFiles/SimpleRender.dir/src/command/command.c.s

CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o: src/config/descriptor.c
CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o -MF CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o.d -o CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/descriptor.c

CMakeFiles/SimpleRender.dir/src/config/descriptor.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/config/descriptor.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/descriptor.c > CMakeFiles/SimpleRender.dir/src/config/descriptor.c.i

CMakeFiles/SimpleRender.dir/src/config/descriptor.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/config/descriptor.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/descriptor.c -o CMakeFiles/SimpleRender.dir/src/config/descriptor.c.s

CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o: src/config/pipeconfig.c
CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o -MF CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o.d -o CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/pipeconfig.c

CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/pipeconfig.c > CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.i

CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/pipeconfig.c -o CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.s

CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o: src/config/renderpass.c
CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o -MF CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o.d -o CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/renderpass.c

CMakeFiles/SimpleRender.dir/src/config/renderpass.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/config/renderpass.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/renderpass.c > CMakeFiles/SimpleRender.dir/src/config/renderpass.c.i

CMakeFiles/SimpleRender.dir/src/config/renderpass.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/config/renderpass.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/renderpass.c -o CMakeFiles/SimpleRender.dir/src/config/renderpass.c.s

CMakeFiles/SimpleRender.dir/src/config/shader.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/config/shader.c.o: src/config/shader.c
CMakeFiles/SimpleRender.dir/src/config/shader.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/SimpleRender.dir/src/config/shader.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/config/shader.c.o -MF CMakeFiles/SimpleRender.dir/src/config/shader.c.o.d -o CMakeFiles/SimpleRender.dir/src/config/shader.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/shader.c

CMakeFiles/SimpleRender.dir/src/config/shader.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/config/shader.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/shader.c > CMakeFiles/SimpleRender.dir/src/config/shader.c.i

CMakeFiles/SimpleRender.dir/src/config/shader.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/config/shader.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/shader.c -o CMakeFiles/SimpleRender.dir/src/config/shader.c.s

CMakeFiles/SimpleRender.dir/src/config/vertex.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/config/vertex.c.o: src/config/vertex.c
CMakeFiles/SimpleRender.dir/src/config/vertex.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/SimpleRender.dir/src/config/vertex.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/config/vertex.c.o -MF CMakeFiles/SimpleRender.dir/src/config/vertex.c.o.d -o CMakeFiles/SimpleRender.dir/src/config/vertex.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/vertex.c

CMakeFiles/SimpleRender.dir/src/config/vertex.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/config/vertex.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/vertex.c > CMakeFiles/SimpleRender.dir/src/config/vertex.c.i

CMakeFiles/SimpleRender.dir/src/config/vertex.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/config/vertex.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/config/vertex.c -o CMakeFiles/SimpleRender.dir/src/config/vertex.c.s

CMakeFiles/SimpleRender.dir/src/init/init.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/init/init.c.o: src/init/init.c
CMakeFiles/SimpleRender.dir/src/init/init.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/SimpleRender.dir/src/init/init.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/init/init.c.o -MF CMakeFiles/SimpleRender.dir/src/init/init.c.o.d -o CMakeFiles/SimpleRender.dir/src/init/init.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/init/init.c

CMakeFiles/SimpleRender.dir/src/init/init.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/init/init.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/init/init.c > CMakeFiles/SimpleRender.dir/src/init/init.c.i

CMakeFiles/SimpleRender.dir/src/init/init.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/init/init.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/init/init.c -o CMakeFiles/SimpleRender.dir/src/init/init.c.s

CMakeFiles/SimpleRender.dir/src/memory/memory.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/memory/memory.c.o: src/memory/memory.c
CMakeFiles/SimpleRender.dir/src/memory/memory.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/SimpleRender.dir/src/memory/memory.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/memory/memory.c.o -MF CMakeFiles/SimpleRender.dir/src/memory/memory.c.o.d -o CMakeFiles/SimpleRender.dir/src/memory/memory.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/memory/memory.c

CMakeFiles/SimpleRender.dir/src/memory/memory.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/memory/memory.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/memory/memory.c > CMakeFiles/SimpleRender.dir/src/memory/memory.c.i

CMakeFiles/SimpleRender.dir/src/memory/memory.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/memory/memory.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/memory/memory.c -o CMakeFiles/SimpleRender.dir/src/memory/memory.c.s

CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o: src/pipeline/pipeline.c
CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o -MF CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o.d -o CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/pipeline/pipeline.c

CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/pipeline/pipeline.c > CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.i

CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/pipeline/pipeline.c -o CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.s

CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o: src/pipeline/swap.c
CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o -MF CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o.d -o CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/pipeline/swap.c

CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/pipeline/swap.c > CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.i

CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/pipeline/swap.c -o CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.s

CMakeFiles/SimpleRender.dir/src/texture/texture.c.o: CMakeFiles/SimpleRender.dir/flags.make
CMakeFiles/SimpleRender.dir/src/texture/texture.c.o: src/texture/texture.c
CMakeFiles/SimpleRender.dir/src/texture/texture.c.o: CMakeFiles/SimpleRender.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/SimpleRender.dir/src/texture/texture.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SimpleRender.dir/src/texture/texture.c.o -MF CMakeFiles/SimpleRender.dir/src/texture/texture.c.o.d -o CMakeFiles/SimpleRender.dir/src/texture/texture.c.o -c /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/texture/texture.c

CMakeFiles/SimpleRender.dir/src/texture/texture.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SimpleRender.dir/src/texture/texture.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/texture/texture.c > CMakeFiles/SimpleRender.dir/src/texture/texture.c.i

CMakeFiles/SimpleRender.dir/src/texture/texture.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SimpleRender.dir/src/texture/texture.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elijah/Desktop/Dev/Git/repos/SimpleRender/src/texture/texture.c -o CMakeFiles/SimpleRender.dir/src/texture/texture.c.s

# Object files for target SimpleRender
SimpleRender_OBJECTS = \
"CMakeFiles/SimpleRender.dir/src/command/command.c.o" \
"CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o" \
"CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o" \
"CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o" \
"CMakeFiles/SimpleRender.dir/src/config/shader.c.o" \
"CMakeFiles/SimpleRender.dir/src/config/vertex.c.o" \
"CMakeFiles/SimpleRender.dir/src/init/init.c.o" \
"CMakeFiles/SimpleRender.dir/src/memory/memory.c.o" \
"CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o" \
"CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o" \
"CMakeFiles/SimpleRender.dir/src/texture/texture.c.o"

# External object files for target SimpleRender
SimpleRender_EXTERNAL_OBJECTS =

libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/command/command.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/config/descriptor.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/config/pipeconfig.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/config/renderpass.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/config/shader.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/config/vertex.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/init/init.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/memory/memory.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/pipeline/pipeline.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/pipeline/swap.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/src/texture/texture.c.o
libSimpleRender.a: CMakeFiles/SimpleRender.dir/build.make
libSimpleRender.a: CMakeFiles/SimpleRender.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking C static library libSimpleRender.a"
	$(CMAKE_COMMAND) -P CMakeFiles/SimpleRender.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SimpleRender.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SimpleRender.dir/build: libSimpleRender.a
.PHONY : CMakeFiles/SimpleRender.dir/build

CMakeFiles/SimpleRender.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SimpleRender.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SimpleRender.dir/clean

CMakeFiles/SimpleRender.dir/depend:
	cd /home/elijah/Desktop/Dev/Git/repos/SimpleRender && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/elijah/Desktop/Dev/Git/repos/SimpleRender /home/elijah/Desktop/Dev/Git/repos/SimpleRender /home/elijah/Desktop/Dev/Git/repos/SimpleRender /home/elijah/Desktop/Dev/Git/repos/SimpleRender /home/elijah/Desktop/Dev/Git/repos/SimpleRender/CMakeFiles/SimpleRender.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SimpleRender.dir/depend

