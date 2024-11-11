#ifndef FLOCK_H
#define FLOCK_H

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "shapes/boid.h"

std::unordered_map<std::tuple<int, int, int>, glm::vec3> getCenter(const std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>>& boid_map) {
    std::unordered_map<std::tuple<int, int, int>, glm::vec3> flock_map;
    for (const auto& [cell, boids] : boid_map) {
      glm::vec3 sum(0.0f,0.0f,0.0f);
      float total = 0.1f;
      for(const Boid& boid : boids){
        sum += boid.getPos();
        total++;
      }
      flock_map[cell] = sum / total;
    }
    return flock_map;
}



#endif // !FLOCK_H
