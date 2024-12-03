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
#include "utils/generation.h"
#include "utils/sound.h"
#include "algorithm/flock.h"
#include "shapes/collectible.h"



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


    Player player(0.15f, glm::vec3(100.0f,0.0f,0.0f));


    int worldSize = 50;
    std::unordered_map<std::tuple<int, int, int>, std::vector<Obstacle*>> box_map = generateRandomBoxes(1,1,worldSize);
    std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>> boid_map;
    generateRandomBoids(boid_map, 1, worldSize, box_map, 0, player.getPos());
    generateRandomBoids(boid_map, 0, worldSize, box_map, 0, player.getPos());
    generateRandomBoids(boid_map, 0, worldSize, box_map, 0, player.getPos());

    std::vector<Bullet> bullets;
    std::vector<Collectible> collectibles;
    GLuint asteroidTexture = loadTexture("../assets/asteroid.jpg");
    GLuint sunTexture = loadTexture("../assets/sun.jpg");


    Shader lightingShader("../shaders/shadow.vs", "../shaders/shadow.fs");
    Shader textureShader("../shaders/texture.vs", "../shaders/texture.fs");
    Shader brightShader("../shaders/1.colors.vs", "../shaders/1.colors.fs");

    Space space(200.0f, 100.0f, 1, 1, player.getPos(), box_map);

    Planet sun(30.0f, glm::vec3(0.0f,0.0f,0.0f), 2.5f);

    Planet earth(20.0f, glm::vec3(0.0f,0.0f,0.0f), 2.5f);
    Planet moon(2.0f, glm::vec3(0.0f,0.0f,0.0f), 0.1f);

    earth.orbit(200.0f, 0.01f);
    moon.orbit(10.0f, 0.05f);

    std::vector<Planet> planets;
    //planets.push_back(sun);
    //planets.push_back(earth);
    //planets.push_back(moon);

    Timer timer;


    glEnable(GL_DEPTH_TEST);

    int num_boids = 0;



    bool game_over = false;
    while (!glfwWindowShouldClose(window) && !game_over) {
        timer.start();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if(shouldSpawnBoid(timer.get_frame()) && num_boids < 200){
          //generateRandomBoids(boid_map, 1, 20.0f, box_map, timer.get_frame(), player.getPos());
        }

        boid_map = recalculateCells(boid_map, num_boids);


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);


        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)width / (float)height, 0.1f, 1000.0f);
        view = glm::lookAt(cameraPos - glm::vec3(0.0f,0.2f,0.0f), cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        glLoadMatrixf(glm::value_ptr(view));

        updateCamera(window, player.getPos());


        brightShader.use();
        brightShader.setMat4("model", model);
        brightShader.setMat4("projection", projection);
        brightShader.setMat4("view", view);
        brightShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);

        //drawChunkBorders(box_map);


        std::unordered_map<std::tuple<int, int, int>, glm::vec3> flock_map = getCenter(boid_map);

        std::tuple<int, int, int> player_cell = positionToCell(player.getPos());

        for(auto& [cell, boids] : boid_map){
          if(game_over)
            break;

          if(player_cell == cell){
            for(Boid& b : boids){
              if(b.contains(player.getPos())){
                game_over = true;
              }
            }
          }

          glm::vec3 cell_flock = flock_map[cell];
          std::vector<Obstacle*> cell_boxes = box_map[cell];

          for (size_t i = 0; i < boids.size(); i++) {
            if(!boids[i].act(player.getPos(),
                  cell_boxes,
                  cell_flock,
                  boids)){
              if(rand() % 10 == 0){
                collectibles.push_back(Collectible(0.05f, boids[i].getPos()));
              }
              boids.erase(boids.begin() + i);
            }
            boids[i].draw(brightShader);
          }
        }

        for(Collectible& c : collectibles){
          c.draw(brightShader);
          if(c.contains(player.getPos())){
            benefit_t collected_benefit = c.collect();
            player.applyBenefit(collected_benefit);
          }
        }

        textureShader.use();
        textureShader.setMat4("model", model);
        textureShader.setMat4("view", view);
        textureShader.setMat4("projection", projection);
        glBindTexture(GL_TEXTURE_2D, asteroidTexture);


        space.render(brightShader, textureShader);


        textureShader.use();
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        //sun.draw();

        lightingShader.use();
        lightingShader.setMat4("model", model);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("cameraPos", cameraPos);
        lightingShader.setVec3("objectColor", 0.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", sun.getPos());
        lightingShader.setVec3("lightColor",  1.0f, 1.0f, 0.75f);

        for (const auto& [cell, boxes] : box_map) {
          for (const Obstacle* box : boxes) {
            box->draw(lightingShader);
            if(box->contains(player.getPos())){
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

          brightShader.use();
          /*
          player.applyForce(
              glm::normalize(planet.getPos() - player.getPos()),
              glm::clamp(planet.gravity / (glm::distance(planet.getPos(), player.getPos()) * 0.4f), 0.0f, 1.0f)
              );
          */
          player.requestOrbit(planet.getPos(), planet.gravity * 20.0f);
          lightingShader.use();
          planet.draw();
          if(planet.contains(player.getPos())){
            game_over = true;
          }
          last = &planet;
        }


        brightShader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);
        brightShader.use();
        player.updatePos(cameraFront);
        player.draw(boid_map, brightShader, frames_since_shot, shot_cooldown);

        processInput(window, player, boid_map);
        glfwSwapBuffers(window);
        glfwPollEvents();
        if(game_over){
          playSound(explosion);
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        timer.record();

    }


    glfwDestroyWindow(window);
    glfwTerminate();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    std::cout << std::endl << "Quitting..." << std::endl;
    return 0;
}

