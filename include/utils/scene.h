#ifndef SCENE_H
#define SCENE_H

#include <optional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <thread>
#include "shapes/player.h"



#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  0.0f);
glm::vec3 playerPos   = glm::vec3(0.0f, 0.0f,  40.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::mat4 view;

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float cameraSpeed = 5.5f;
float radius = 5.0f;  // Radius of orbit around the player

void updateCameraPositionAroundPlayer(glm::vec3 playerPos, float radius, float yaw, float pitch, glm::vec3& cameraPos, glm::vec3& cameraFront) {
    // Convert spherical coordinates to Cartesian coordinates for camera position
    cameraPos.x = playerPos.x + radius * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    cameraPos.y = playerPos.y + radius * sin(glm::radians(pitch));
    cameraPos.z = playerPos.z + radius * cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    // Update the camera front to look towards the player
    cameraFront = glm::normalize(playerPos - cameraPos);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;  // Adjust for sensitivity
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;  // Horizontal movement controls yaw (left-right)
        pitch += yoffset;  // Vertical movement controls pitch (up-down)

        // Limit pitch to prevent flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    } else {
        firstMouse = true;
    }
}

void updateCamera(GLFWwindow* window, glm::vec3 playerPos) {
    // Update the camera position around the player, adjusting based on yaw and pitch
    updateCameraPositionAroundPlayer(playerPos, radius, yaw, pitch, cameraPos, cameraFront);
}

void processInput(GLFWwindow *window) {
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameraSpeed = 10.5f * deltaTime;


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        playerPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        playerPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        playerPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        playerPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        playerPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        playerPos -= cameraSpeed * cameraUp;

    float sensitivity = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        yaw -= sensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        yaw += sensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        pitch += sensitivity;
        if (pitch > 89.0f) pitch = 89.0f;  // Limit pitch to prevent flipping
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        pitch -= sensitivity;
        if (pitch < -89.0f) pitch = -89.0f;  // Limit pitch to prevent flipping
    }

    float zoom_rate = 0.2f;

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        radius += zoom_rate;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        radius -= zoom_rate;
}



int width = 0;
int height = 0;

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f
};


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

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  // Flip texture vertically
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

#endif
