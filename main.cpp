#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
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
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <thread>
#include <chrono>


#include "shapes/sphere.h"
#include "shapes/box.h"
#include "shapes/boid.h"
#include "shapes/space.h"
#include "shapes/bullet.h"
#include "utils/generation.h"
#include "utils/sound.h"
#include "algorithm/flock.h"


// Red Boid parameters
BoidParams redBoidParams = {
    1.0f, 0.0f, 0.0f,    // boid color: red
    1.0f, 0.5f, 0.5f,    // trail color: light red
    0.003f,             // rayStepSize
    0.03f,              // rayMaxLength
    0.85f,               // forceApplicationCoefficient
    0.001f,              // speedIncreaseCoefficient
    10.0f,                // obstacleRepelForce
    3.0f,               // obstacleRepelDecay
    1.0f,                // goalAttraction
    0.2f                 // size
};

// Blue Boid parameters
BoidParams blueBoidParams = {
    0.0f, 0.0f, 1.0f,    // boid color: blue
    0.3f, 0.3f, 1.0f,    // trail color: light blue
    0.003f,            // rayStepSize
    0.03f,             // rayMaxLength
    0.7f,                // forceApplicationCoefficient
    0.001f,              // speedIncreaseCoefficient
    10.5f,               // obstacleRepelForce
    3.5f,                // obstacleRepelDecay
    1.0f,                // goalAttraction
    0.1f                 // size
};

// Green Boid parameters
BoidParams greenBoidParams = {
    0.0f, 1.0f, 0.0f,    // boid color: green
    0.3f, 1.0f, 0.3f,    // trail color: light green
    0.003f,            // rayStepSize
    0.03f,             // rayMaxLength
    0.8f,                // forceApplicationCoefficient
    0.001f,              // speedIncreaseCoefficient
    10.8f,                // obstacleRepelForce
    4.2f,                // obstacleRepelDecay
    1.0f,                // goalAttraction
    0.07f                // size
};



int main() {
    std::optional<GLFWwindow*> opt_window = init_scene();
    if(!opt_window.has_value()){
      return -1;
    }
    GLFWwindow* window = opt_window.value();
    glm::vec3 lightPos  = glm::vec3(0.0f, 10.0f,  0.0f);


    initMixer();
    int lazer = loadSound("../assets/lazer.wav");
    int explosion = loadSound("../assets/explosion.wav");

    //std::this_thread::sleep_for(std::chrono::seconds(3));


    Player player(0.15f, cameraPos + glm::vec3(0.0f, 0.0f, 3.0f));

    glm::vec3 offset(0.0f, 0.0f, 1.0f);

    glm::vec3 relativePlayerPos = cameraPos +
                                  offset.x +
                                  offset.y * cameraUp +
                                  offset.z;

    glm::vec3 interpolatedPos = glm::mix(player.getPos(), relativePlayerPos, 0.1f);  // Adjust 0.1f for smoother transitions
    player.updatePos(relativePlayerPos);

    int worldSize = 20;
    std::unordered_map<std::tuple<int, int, int>, std::vector<Box>> box_map = generateRandomBoxes(1,1,worldSize);
    std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>> boid_map;
    generateRandomBoids(boid_map, 1, worldSize, box_map, redBoidParams);
    generateRandomBoids(boid_map, 1, worldSize, box_map, greenBoidParams);
    generateRandomBoids(boid_map, 1, worldSize, box_map, blueBoidParams);

    std::vector<Bullet> bullets;
    GLuint boidTexture = loadTexture("../assets/boid.jpg");
    GLuint playerTexture = loadTexture("../assets/player.jpg");


    Shader lightingShader("../shaders/shadow.vs", "../shaders/shadow.fs");
    Shader textureShader("../shaders/boid.vs", "../shaders/boid.fs");
    Shader brightShader("../shaders/1.colors.vs", "../shaders/1.colors.fs");

    Space space(150.0f, loadTexture("../assets/space.png"));


    glEnable(GL_DEPTH_TEST);



    bool game_over = false;
    int frame = 0;
    while (!glfwWindowShouldClose(window) && !game_over) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(frame % static_cast<int>(CELL_SIZE) == 0){
          boid_map = recalculateCells(boid_map);
        }

        frame++;


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);


        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 200.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        glLoadMatrixf(glm::value_ptr(view));

        glm::vec3 interpolatedPos = glm::mix(player.getPos(), playerPos, 0.1f);
        player.updatePos(interpolatedPos);

        updateCamera(window, player.getPos());


        brightShader.use();
        brightShader.setMat4("model", model);
        brightShader.setMat4("projection", projection);
        brightShader.setMat4("view", view);
        brightShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);

        glm::vec3 flock_center = getCenter(boid_map);

        for(auto& [cell, boids] : boid_map){
          if(game_over)
            break;

          for (size_t i = 0; i < boids.size(); i++) {
            if(!boids[i].act(interpolatedPos,
                  box_map[positionToCell(boids[i].getPos())],
                  flock_center,
                  boid_map[positionToCell(boids[i].getPos())])){
              std::cout << "Alert: boid crashed!!" << std::endl;
              boids.erase(boids.begin() + i);
            }
            boids[i].draw(brightShader);
            if(boids[i].contains(interpolatedPos)){
              playSound(explosion);
              game_over = true;
              break;
            }
          }
        }


        brightShader.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        for(auto& b : bullets){
          b.updatePosition(interpolatedPos);
          b.checkCollisions(boid_map[positionToCell(b.getPos())]);
          b.draw();
        }


        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS){
          bullets.push_back(Bullet(interpolatedPos, cameraFront, 0.3f, explosion));
          playSound(lazer);
        }


        textureShader.use();
        textureShader.setMat4("model", model);
        textureShader.setMat4("view", view);
        textureShader.setMat4("projection", projection);

        space.render();


        glBindTexture(GL_TEXTURE_2D, boidTexture);
        player.draw(brightShader);

        lightingShader.use();
        lightingShader.setMat4("model", model);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("cameraPos", cameraPos);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("objectColor", 0.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);

        for (const auto& [cell, boxes] : box_map) {
          for (const Box& box : boxes) {
            box.draw(lightingShader);
            if(box.contains(interpolatedPos)){
              game_over = true;
              break;
            }
          }
        }

        processInput(window, player);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    return 0;
}

