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
#include "shapes/player.h"
#include <ctime>
#include <random>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>



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

std::vector<Boid> generateRandomBoids(int count, std::vector<Box> boxes){
    std::vector<Boid> boids;
    for (int i = 0; i < count; ++i) {
        boids.push_back(Boid(0.1f, getRandomPointOutsideBoxes(boxes, 8, 0)));
    }
    return boids;
}


int main() {
    std::optional<GLFWwindow*> opt_window = init_scene();
    if(!opt_window.has_value()){
      return -1;
    }
    GLFWwindow* window = opt_window.value();



    glm::vec3 lightPos  = glm::vec3(0.0f, 0.0f,  0.0f);

    std::vector<Box> boxes = generateRandomBoxes(200,1,30);

    glm::vec3 goal_pos = getRandomPointOutsideBoxes(boxes, 5);

    Player player(0.15f, cameraPos + glm::vec3(0.0f, 0.0f, 3.0f));

    glm::vec3 offset(0.0f, 0.0f, 1.0f);

    glm::vec3 relativePlayerPos = cameraPos +
                                  offset.x +
                                  offset.y * cameraUp +
                                  offset.z;

    glm::vec3 interpolatedPos = glm::mix(player.getPos(), relativePlayerPos, 0.1f);  // Adjust 0.1f for smoother transitions
    player.updatePos(relativePlayerPos);

    int startingBoids = 200;
    std::vector<Boid> boids = generateRandomBoids(startingBoids, boxes);
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


        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        for(auto box : boxes){
          box.draw();
        }

        lightingShader.setVec3("objectColor", 0.0f, 0.8f, 1.0f);
        for(int i = 0; i < boids.size(); i++){
          if(!boids[i].act(playerPos, boxes)){
            std::cout << "Alert: boid crashed!!" << std::endl;
            boids.erase(boids.begin() + i);
            std::cout << "Boids remaining: " << std::to_string(boids.size()) << std::endl;
          }
          boids[i].draw();
        }

        lightingShader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);

        goal.draw();



        glm::vec3 interpolatedPos = glm::mix(player.getPos(), playerPos, 0.1f);
        player.updatePos(interpolatedPos);

        updateCamera(window, player.getPos());

        player.draw();
        player.shoot(boids);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

