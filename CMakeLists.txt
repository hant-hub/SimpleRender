cmake_minimum_required(VERSION 3.10)

project(SimpleRender)

set(C_LD_FLAGS "-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lm")
set(CMAKE_CXX_FLAGS "${C_LD_FLAGS} -MMD -MP -lm")

file(GLOB_RECURSE SRCS ./src/*.c)
add_subdirectory(src)
add_library(SimpleRender STATIC ${SRCS} ${INC})
target_include_directories(SimpleRender PUBLIC ./include/renderers)
target_include_directories(SimpleRender PUBLIC ./include/core)
target_include_directories(SimpleRender PUBLIC ./lib)
target_include_directories(SimpleRender PUBLIC ./lib/SimpleMath/src/include)
target_include_directories(SimpleRender PUBLIC ./lib/SimpleMath/src/include/mat)
target_include_directories(SimpleRender PUBLIC ./lib/SimpleMath/src/include/vec)
target_include_directories(SimpleRender PUBLIC ./lib/SimpleMath/src/include/trig)
