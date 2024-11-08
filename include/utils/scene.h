#ifndef SCENE_H
#define SCENE_H

#include <optional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int width = 0;
int height = 0;

void initializeOpenGL() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);  // Enable depth testing
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark gray background
}

void setProjection(int width, int height) {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));  // Load the matrix into OpenGL
}

std::optional<GLFWwindow*> init_scene(){
    if (!glfwInit()) {
      std::cerr << "Failed to initialize GLFW" << std::endl;
      return std::nullopt;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        std::cerr << "Failed to get primary monitor" << std::endl;
        glfwTerminate();
      return std::nullopt;
    }

    // Get the video mode (resolution) of the primary monitor
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        std::cerr << "Failed to get video mode" << std::endl;
        glfwTerminate();
      return std::nullopt;
    }

    // Use the monitor's resolution for the window size
    width = mode->width;
    height = mode->height;

    // Create the window with the maximum resolution
    GLFWwindow* window = glfwCreateWindow(width, height, "Sphere Renderer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
      return std::nullopt;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);

    initializeOpenGL();

    setProjection(width, height);
    return std::optional(window);
}


#endif
