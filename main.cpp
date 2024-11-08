#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shapes/sphere.h"
#include "shapes/box.h"
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
    Sphere boid(0.1f, 18, 18, 0.0f, 0.0f, 0.0f);
    Sphere goal(0.1f, 18, 18, 5.0f, 5.0f, 5.0f);
    float s_x = 0.0f;
    float b_x = 10.0f;
    //Box box(5.0f);
    std::vector<Box> boxes = generateRandomBoxes(40,10,60);

    glEnable(GL_DEPTH_TEST);

    glm::vec3 start(0.0f, 0.0f, 0.0f);
    glm::vec3 goal_pos(goal.getX(), goal.getY(), goal.getZ());

    Shader lightingShader("../shaders/shadow.vs", "../shaders/shadow.fs");

    std::vector<glm::vec3> path = aStar(start, goal_pos, boxes);

    std::cout << "path: " << path.size() << std::endl;

    int frame = 0;


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
        if (!path.empty()) {
          drawPath(path);
        }
        goal.draw();

        s_x += 0.01f;
        b_x -= 0.001f;
        boid.setPosition(s_x, 0.0f, 0.0f);
        lightPos = glm::vec3(s_x, 0.3f, 0.0f);

        if(frame % 10 == 0){
          start = glm::vec3(s_x, 0.0f,0.0f);
          path = aStar(start, goal_pos, boxes);
          std::cout << "path: " << path.size() << std::endl;
        }
        frame++;


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

