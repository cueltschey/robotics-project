cmake_minimum_required(VERSION 3.10)
project(KDTree3D)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Include directories for GLFW (if it's not already in default locations)
include_directories(/usr/include/GLFW)

# Link directories for GLFW (where the libglfw.so file is)
link_directories(/usr/lib/x86_64-linux-gnu)

# Add the source files
add_executable(kdtree kdtree.cpp)

# Link libraries
target_link_libraries(kdtree OpenGL::GL GLEW::GLEW glfw)
