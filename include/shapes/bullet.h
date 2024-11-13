#ifndef BULLET_H
#define BULLET_H

#include <glm/glm.hpp>
#include <vector>
#include <tuple>
#include "shapes/sphere.h"
#include "shapes/boid.h"
#include <tuple>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>

class Bullet {
public:
    Bullet(glm::vec3 startPos, glm::vec3 cameraFront,
            std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map);

    

    // Function to draw the bullet
    void draw();

    glm::vec3 getPos() const { return position; };

    // Bullet sphere
    bool gone = false;
    float colorFade = 0.0f;
    
private:
    void drawLine(glm::vec3 start, glm::vec3 end);
    std::tuple<int, int, int> positionToCell(const glm::vec3& pos);
    glm::vec3 position;  // Bullet position
    glm::vec3 direction; // Direction of the bullet (camera front)
    int maxDistance;
    std::vector<glm::vec3> trail;
};

#endif

