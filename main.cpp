#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shapes/sphere.h"
#include "shapes/box.h"
#include "shapes/boid.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/camera.h"
#include <optional>
#include "utils/m_shader.h"
#include "utils/scene.h"
#include <ctime>
#include <random>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>
#include "algorithm/astar.h"


glm::vec3 getRandomPointOutsideBoxes(const std::vector<Box>& boxes, float maxPosition, float minDistance = 0.5f) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-maxPosition, maxPosition);

    glm::vec3 randomPoint;
    bool isValid;

    do {
        // Generate a random point
        randomPoint = glm::vec3(posDist(gen), posDist(gen), posDist(gen));
        isValid = true;

        // Check the point against each box to ensure it’s outside by at least minDistance
        for (const Box& box : boxes) {
            glm::vec3 boxMin(box.getX() - box.getWidth() / 2 - minDistance,
                             box.getY() - box.getHeight() / 2 - minDistance,
                             box.getZ() - box.getDepth() / 2 - minDistance);

            glm::vec3 boxMax(box.getX() + box.getWidth() / 2 + minDistance,
                             box.getY() + box.getHeight() / 2 + minDistance,
                             box.getZ() + box.getDepth() / 2 + minDistance);

            // Check if the point is within the box bounds extended by minDistance
            if (randomPoint.x >= boxMin.x && randomPoint.x <= boxMax.x &&
                randomPoint.y >= boxMin.y && randomPoint.y <= boxMax.y &&
                randomPoint.z >= boxMin.z && randomPoint.z <= boxMax.z) {
                isValid = false;
                break;
            }
        }
    } while (!isValid); // Repeat until a valid point is found

    return randomPoint;
}

std::vector<Box> generateRandomBoxes(int numBoxes, float maxSize, float maxPosition) {
    std::vector<Box> boxes;

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the random distributions
    std::uniform_real_distribution<float> sizeDist(0.5f, maxSize);        // Box size range
    std::uniform_real_distribution<float> posDist(-maxPosition, maxPosition); // Position range

    // Create random boxes
    for (int i = 0; i < numBoxes; ++i) {
        float width = sizeDist(gen);
        float height = sizeDist(gen);
        float depth = sizeDist(gen);

        float x = posDist(gen);
        float y = posDist(gen);
        float z = posDist(gen);

        boxes.push_back(Box(width, height, depth, x, y, z));
    }

    return boxes;
}


int main() {
    std::optional<GLFWwindow*> opt_window = init_scene();
    if(!opt_window.has_value()){
      return -1;
    }
    GLFWwindow* window = opt_window.value();



    glm::vec3 lightPos  = glm::vec3(0.0f, 0.0f,  0.0f);

    std::vector<Box> boxes = generateRandomBoxes(10,2,5);

    glm::vec3 boid_start = getRandomPointOutsideBoxes(boxes, 5);
    cameraPos = boid_start + glm::vec3(0.0f,0.0f,1.0f);
    glm::vec3 goal_pos = getRandomPointOutsideBoxes(boxes, 5);
    Boid boid(0.1f, boid_start);
    Sphere goal(0.1f, goal_pos);
    goal_pos = glm::vec3(goal.getX(), goal.getY(), goal.getZ());


    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("../shaders/shadow.vs", "../shaders/shadow.fs");



    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);

        processInput(window);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", cameraPos);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(view));


        boid.draw();

        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        for(auto box : boxes){
          box.draw();
        }

        lightingShader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);

        ray_points.clear();
        for(auto dir : directions){
          glm::vec3 rayPosition = boid.getPos();

          float stepSize = 0.0001f;
          float maxDistance = 0.01f;
          for (float distance = 0.0f; distance <= maxDistance; distance += stepSize) {
              rayPosition += dir;

              // Check for collision with any box
              auto collisionBox = std::find_if(boxes.begin(), boxes.end(), [&](const Box& box) {
                  return box.contains(rayPosition);
              });

              // If a collision is found, record the point and stop this ray
              if (collisionBox != boxes.end()) {
                  ray_points.push_back(rayPosition);
                  break;
              }
          }
        }

        glm::vec3 new_direction = glm::normalize(goal_pos - boid.getPos());
        boid.applyForce(new_direction, 1.0f);
        boid.applyForce(-new_direction, 2.0f / glm::distance(goal_pos, boid.getPos()));
        boid.updatePos();

        goal.draw();

        lightPos = glm::vec3(boid.getX(), boid.getY() + 0.3f, boid.getZ());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

