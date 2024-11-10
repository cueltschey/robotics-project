#ifndef FLOCK_H
#define FLOCK_H

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "shapes/boid.h"

glm::vec3 getCenter(const std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>>& boid_map) {
    glm::vec3 sum(0.0f);
    float total = 0.0f;
    for (const auto& [cell, boids] : boid_map) {
      for(const Boid& boid : boids){
        sum += boid.getPos();
        total++;
      }
    }
    return sum / total;
}



#endif // !FLOCK_H
