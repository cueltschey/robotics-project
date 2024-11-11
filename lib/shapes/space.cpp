#include "shapes/space.h"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <functional>
#include <random>




Space::Space(float radius_, int numStars_, glm::vec3 playerPosition)
    : radius(radius_), numStars(numStars_) {
      createStars(playerPosition);
    }


// Render the background sphere
void Space::render() {
  for(Sphere& star : stars){
    star.draw();
  }
}

void Space::createStars(const glm::vec3& playerPosition){
  for (int i = 0; i < numStars; i++) {
    stars.push_back(Sphere(0.1f, randomStarPos(playerPosition, radius / 2, radius * 2), 0.0f));
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
