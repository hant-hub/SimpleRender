# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := test

BUILD_DIR := ./build
SRC_DIRS := ./src
INC_SEARCH = ./src ./lib
SHADER_DIR := ./shaders


# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
SHADERS := $(shell find $(SHADER_DIRS) -name '*.glsl')
SHADER_NAMES := $(notdir $(SHADERS))

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
COMPSHADERS := $(SHADER_NAMES:%=$(BUILD_DIR)/shaders/%.spv)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(INC_SEARCH)/ -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CFLAGS := $(INC_FLAGS) -MMD -MP -D $(BUILD)

LDFLAGS = -Werror -Wall -Wextra -pedantic -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi


# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) $(COMPSHADERS) clean
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	
# Shaders
$(BUILD_DIR)/shaders/%.spv: $(SHADER_DIR)/%.glsl 
	mkdir -p $(dir $@)
	glslc $< -o $@  


# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@



clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/$(TARGET_EXEC)
	clear
	compiledb -n make
	cd $(BUILD_DIR); \
	./$(notdir $<); \
	cd ..;
	

.PHONY: clean test

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
