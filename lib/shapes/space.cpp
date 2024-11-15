#include "shapes/space.h"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <functional>
#include <random>
#include "utils/generation.h"




Space::Space(float stars_radius_,
    float asteroids_radius_,
    int numStars_,
    int numAsteroids_,
    glm::vec3 playerPosition,
    std::unordered_map<std::tuple<int, int, int>, std::vector<Obstacle*>>& box_map)
    : stars_radius(stars_radius_), asteroids_radius(asteroids_radius_), numStars(numStars_), numAsteroids(numAsteroids_) {

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<float> asteroidSizeDist(0.5f, 2.0f);
      for (int i = 0; i < numStars; i++) {
        stars.push_back(Sphere(0.1f, randomStarPos(playerPosition, stars_radius / 2, stars_radius * 2), 0.0f));
      }
      for (int i = 0; i < numAsteroids; i++) {
        glm::vec3 randomPos = randomAsteroidPos(playerPosition, asteroids_radius / 2, asteroids_radius * 2);
        box_map[positionToCell(randomPos)].push_back(new Asteroid(asteroidSizeDist(gen), randomPos, 0.0f));
      }
    }


// Render the background sphere
void Space::render(Shader& lightShader, Shader& textureShader){
  lightShader.use();
  lightShader.setVec3("objectColor", glm::vec3(1.0f,1.0f,1.0f));
  for(Sphere& star : stars){
    star.draw();
  }
}


glm::vec3 Space::randomStarPos(const glm::vec3& playerPosition, float minDistance, float maxDistance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-maxDistance, maxDistance);

    glm::vec3 randomPoint;
    bool isValid;

    do {
        // Generate a random point
        randomPoint = glm::vec3(posDist(gen), posDist(gen), posDist(gen));
        isValid = true;
        if(glm::distance(randomPoint, playerPosition) <= minDistance){
          isValid = false;
        }
    } while (!isValid); // Repeat until a valid point is found
    return randomPoint;
}


glm::vec3 Space::randomAsteroidPos(const glm::vec3& playerPosition, float minDistance, float maxDistance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-maxDistance, maxDistance);

    glm::vec3 randomPoint;
    bool isValid;

    do {
        // Generate a random point
        randomPoint = glm::vec3(posDist(gen), posDist(gen), posDist(gen));
        isValid = true;
        if(glm::distance(randomPoint, playerPosition) <= minDistance){
          isValid = false;
        }
    } while (!isValid); // Repeat until a valid point is found
    return randomPoint;
}
