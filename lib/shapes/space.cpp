#include "shapes/space.h"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <functional>
#include <random>




Space::Space(float stars_radius_, float asteroids_radius_, int numStars_, int numAsteroids_, glm::vec3 playerPosition)
    : stars_radius(stars_radius_), asteroids_radius(asteroids_radius_), numStars(numStars_), numAsteroids(numAsteroids_) {
      createStars(playerPosition);
    }


// Render the background sphere
void Space::render(Shader& lightShader, Shader& textureShader){
  textureShader.use();
  for(Asteroid& asteroid: asteroids){
    asteroid.draw();
  }

  lightShader.setVec3("objectColor", glm::vec3(1.0f,1.0f,1.0f));
  lightShader.use();
  for(Sphere& star : stars){
    star.draw();
  }
}

void Space::createStars(const glm::vec3& playerPosition){
  for (int i = 0; i < numStars; i++) {
    stars.push_back(Sphere(0.1f, randomStarPos(playerPosition, stars_radius / 2, stars_radius * 2), 0.0f));
  }
  for (int i = 0; i < numAsteroids; i++) {
    asteroids.push_back(Asteroid(1.0f, randomAsteroidPos(playerPosition, asteroids_radius / 2, asteroids_radius * 2), 0.0f));
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
