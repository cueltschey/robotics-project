cmake_minimum_required(VERSION 3.10)
project(MovingTriangleAStar)


set(glm_DIR /lib/cmake/glm)
set(glfw3_DIR /usr/lib/x86_64-linux-gnu/cmake/glfw3)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# Download and build GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

include_directories(${OPENGL_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} /usr/include/GLFW)
file(GLOB LIB_SRC lib/*.cpp)
include_directories(include)
add_executable(boids main.cpp ${LIB_SRC})
target_link_libraries(boids ${OPENGL_gl_LIBRARY} glfw glm::glm GLEW)

