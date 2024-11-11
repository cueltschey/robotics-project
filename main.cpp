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


#include "utils/timer.h"
#include "shapes/sphere.h"
#include "shapes/sun.h"
#include "shapes/planet.h"
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


    initMixer();
    int lazer = loadSound("../assets/lazer.wav");
    int explosion = loadSound("../assets/explosion.wav");

    //std::this_thread::sleep_for(std::chrono::seconds(3));


    Player player(0.15f, playerPos);

    glm::vec3 interpolatedPos = glm::mix(player.getPos(), cameraPos, 0.1f);  // Adjust 0.1f for smoother transitions
    player.updatePos(cameraPos);

    int worldSize = 5;
    std::unordered_map<std::tuple<int, int, int>, std::vector<Box>> box_map = generateRandomBoxes(1,1,worldSize);
    std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>> boid_map;
    generateRandomBoids(boid_map, 200, worldSize, box_map, redBoidParams);
    generateRandomBoids(boid_map, 200, worldSize, box_map, greenBoidParams);
    generateRandomBoids(boid_map, 200, worldSize, box_map, blueBoidParams);

    std::vector<Bullet> bullets;
    ////GLuint boidTexture = loadTexture("../assets/boid.jpg");
    GLuint playerTexture = loadTexture("../assets/player.jpg");


    Shader lightingShader("../shaders/shadow.vs", "../shaders/shadow.fs");
    Shader textureShader("../shaders/boid.vs", "../shaders/boid.fs");
    Shader brightShader("../shaders/1.colors.vs", "../shaders/1.colors.fs");

    Space space(200.0f, 1000, player.getPos());

    Planet sun(5.0f, glm::vec3(0.0f,0.0f,0.0f));

    Planet earth(2.0f, glm::vec3(50.0f,0.0f,0.0f));
    Planet moon(0.5f, glm::vec3(0.0f,0.0f,0.0f));

    earth.orbit(50.0f, 0.1f);
    moon.orbit(10.0f, 0.5f);

    std::vector<Planet> planets;
    planets.push_back(sun);
    planets.push_back(earth);
    planets.push_back(moon);

    Timer t;


    glEnable(GL_DEPTH_TEST);



    bool game_over = false;
    int frame = 0;
    while (!glfwWindowShouldClose(window) && !game_over) {
        t.start();
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


        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)width / (float)height, 0.1f, 800.0f);
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

        std::unordered_map<std::tuple<int, int, int>, glm::vec3> flock_map = getCenter(boid_map);

        std::tuple<int, int, int> player_cell = positionToCell(interpolatedPos);

        for(auto& [cell, boids] : boid_map){
          if(game_over)
            break;

          if(player_cell == cell){
            for(Boid& b : boids){
              if(b.contains(interpolatedPos)){
                playSound(explosion);
                game_over = true;
                break;
              }
            }
          }

          glm::vec3 cell_flock = flock_map[cell];
          std::vector<Box> cell_boxes = box_map[cell];

          for (size_t i = 0; i < boids.size(); i++) {
            if(!boids[i].act(interpolatedPos,
                  cell_boxes,
                  cell_flock,
                  boids)){
              std::cout << "Alert: boid crashed!!" << std::endl;
              boids.erase(boids.begin() + i);
            }
            boids[i].draw(brightShader);
          }
          std::cout <<  boids.size() << std::endl;
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


        //textureShader.use();
        //textureShader.setMat4("model", model);
        //textureShader.setMat4("view", view);
        //textureShader.setMat4("projection", projection);


        brightShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        space.render();



        player.draw(brightShader);

        brightShader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);
        sun.draw();

        lightingShader.use();
        lightingShader.setMat4("model", model);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("cameraPos", cameraPos);
        lightingShader.setVec3("objectColor", 0.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", sun.getPos());
        lightingShader.setVec3("lightColor",  1.0f, 1.0f, 0.75f);

        for (const auto& [cell, boxes] : box_map) {
          for (const Box& box : boxes) {
            box.draw(lightingShader);
            if(box.contains(interpolatedPos)){
              playSound(explosion);
              game_over = true;
            }
          }
        }

        lightingShader.setVec3("objectColor", 0.5f, 0.5f, 0.5f);
        Planet* last = nullptr;
        for(Planet& planet : planets){
          if(last != nullptr){
            planet.updatePos(last->getPos());
          }
          player.applyForce(
              planet.getPos() - interpolatedPos,
              planet.radius / distance(planet.getPos(), interpolatedPos)
              );
          planet.draw();
          if(planet.contains(interpolatedPos)){
            playSound(explosion);
            game_over = true;
          }
          last = &planet;
        }

        processInput(window, player);
        glfwSwapBuffers(window);
        glfwPollEvents();
        if(game_over){
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        //t.stop();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    return 0;
}

