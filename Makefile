# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := sr.a

BUILD_DIR := ./build
SRC_DIRS := ./src ./renderers
INC_SEARCH = ./lib ./include
SHADER_DIR := ./shaders
RESOURCE_DIR := ./resources


# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
SHADERS := $(shell find $(SHADER_DIR) -name '*.vert' -o -name '*.frag')
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

BUILD_FLAGS := -g

ifeq ($(BUILD), DEBUG)
	BUILD_FLAGS = -g
endif

ifeq ($(BUILD), RELEASE)
	BUILD_FLAGS = -O2
endif

CFLAGS := $(BUILD_FLAGS) $(INC_FLAGS) -MMD -MP -D $(BUILD)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lm


# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) $(COMPSHADERS) resources
	ar rcs $@ $(OBJS)
	
# Shaders
$(BUILD_DIR)/shaders/%.spv: $(SHADER_DIR)/% 
	mkdir -p $(dir $@)
	glslc $< -o $@  

#resources
resources :
	rm -rf $(BUILD_DIR)/resources
	mkdir -p $(RESOURCE_DIR)
	cp $(RESOURCE_DIR) -r $(BUILD_DIR)/resources 

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/$(TARGET_EXEC)
	clear
	compiledb -n make
	$(CC) ./test/main.c ./$(BUILD_DIR)/$(TARGET_EXEC) $(INC_FLAGS)  -o ./build/test $(LDFLAGS); \
	cd $(BUILD_DIR); \
	./test;

static:
	clang-tidy $(SRCS)
	

.PHONY: clean test resources

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
