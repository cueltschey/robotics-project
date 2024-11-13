#include "shapes/bullet.h"
#include <glm/gtc/matrix_transform.hpp>

#define CELL_SIZE 2.0f

namespace std {
    template<>
    struct hash<std::tuple<int, int, int>> {
        size_t operator()(const std::tuple<int, int, int>& t) const {
            size_t h1 = std::hash<int>{}(std::get<0>(t));
            size_t h2 = std::hash<int>{}(std::get<1>(t));
            size_t h3 = std::hash<int>{}(std::get<2>(t));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}


Bullet::Bullet(glm::vec3 startPos, glm::vec3 cameraFront,
        std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map)
    : position(startPos), 
    direction(glm::normalize(cameraFront)), 
    maxDistance(50)
    {
      int distance = 0;
      while (distance < maxDistance && !gone) {
        trail.push_back(position);
        std::vector<Boid> boids = boid_map[positionToCell(position)];
        for (Boid& boid : boids) {
            if (glm::distance(boid.getPos(), position) < 0.1f){
                boid.explode();
                gone = true;
                break;
            } else{
              glm::vec3 normalized_force = glm::normalize(boid.getPos() - position);
              float strength = 1.0f;
              glm::vec3 force = normalized_force * strength;
              float forceApplicationCoefficient = 1.01f;
              direction += force * forceApplicationCoefficient;
            }
            boid.explode();
        }
        position += direction;
        distance++;
    }
}

void Bullet::draw(){
    if (!gone) {
        colorFade += 0.01f;
        // TODO: draw lines
    }
    if(colorFade >= 1.0f){
      gone = true;
    }
    for(int i = 0; i < trail.size() - 2; i++){
      drawLine(trail[i], trail[i+ 1]);
    }
}

void Bullet::drawLine(glm::vec3 start, glm::vec3 end){
    glBegin(GL_LINES);
    glVertex3fv(glm::value_ptr(start));
    glVertex3fv(glm::value_ptr(end));
    glEnd();
}

std::tuple<int, int, int> Bullet::positionToCell(const glm::vec3& pos) {
    int cellX = static_cast<int>(std::ceil(pos.x) / CELL_SIZE);
    int cellY = static_cast<int>(std::ceil(pos.y) / CELL_SIZE);
    int cellZ = static_cast<int>(std::ceil(pos.z) / CELL_SIZE);

    return std::make_tuple(cellX, cellY, cellZ);
}



