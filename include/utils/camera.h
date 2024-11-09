#ifndef CAMERA_H
#define CAMERA_H

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  5.0f);  // Camera initially positioned 5 units away from the player
glm::vec3 playerPos   = glm::vec3(0.0f, 0.0f,  0.0f);
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

float cameraSpeed = 3.5f;
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
}

#endif
