cmake_minimum_required(VERSION 3.10)
project(boids)

# Set directories for libraries if needed. 
# Ensure these paths exist or adjust as needed for your system.
set(glm_DIR "/lib/cmake/glm")
set(glfw3_DIR "/usr/lib/x86_64-linux-gnu/cmake/glfw3")

cmake_policy(SET CMP0072 NEW)

# Find OpenGL, glm, glfw, GLEW, assimp, and SDL2 libraries
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    message(STATUS "OpenGL found: ${OPENGL_gl_LIBRARY}")
else()
    message(WARNING "OpenGL not found!")
endif()

find_package(glm REQUIRED)
if(glm_FOUND)
    message(STATUS "GLM found: ${GLM_INCLUDE_DIRS}")
else()
    message(WARNING "GLM not found!")
endif()

find_package(glfw3 REQUIRED)
if(glfw3_FOUND)
    message(STATUS "GLFW3 found: ${glfw3_DIR}")
else()
    message(WARNING "GLFW3 not found!")
endif()

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
    message(STATUS "GLEW found")
else()
    message(WARNING "GLEW not found!")
endif()

find_package(assimp REQUIRED)
if(assimp_FOUND)
    message(STATUS "Assimp found")
else()
    message(WARNING "Assimp not found!")
endif()

find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_INCLUDE_DIR}")
else()
    message(WARNING "SDL2 not found!")
endif()

# SDL_mixer library finding
find_library(SDL_MIXER_LIBRARY
  NAMES SDL2_mixer
  HINTS
    ENV SDLMIXERDIR
    ENV SDLDIR
  PATH_SUFFIXES lib
)

if(SDL_MIXER_LIBRARY)
    message(STATUS "SDL2_mixer found: ${SDL_MIXER_LIBRARY}")
else()
    message(WARNING "SDL2_mixer not found!")
endif()

# Optional: Set GLFW build options
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

# Include directories
include_directories(${OPENGL_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} /usr/include/GLFW /usr/include/stb/)

# Add source files
file(GLOB LIB_SRC lib/shapes/*.cpp lib/utils/*.cpp)
include_directories(include)

# Add the executable
add_executable(boids main.cpp ${LIB_SRC})

# Link libraries
target_link_libraries(boids
  ${OPENGL_gl_LIBRARY}
  glfw
  glm::glm
  GLEW
  SDL2
  SDL2_mixer
  assimp
)

