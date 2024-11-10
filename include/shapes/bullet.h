#ifndef BULLET_H
#define BULLET_H

#include <glm/glm.hpp>
#include <vector>
#include "shapes/sphere.h"
#include "shapes/boid.h"

class Bullet {
public:
    Bullet(glm::vec3 startPos, glm::vec3 cameraFront, float speed, int explosion_);
    
    // Function to update the bullet's position
    void updatePosition(glm::vec3 playerPos);

    // Function to check for collisions with boids
    void checkCollisions(std::vector<Boid> boids);
    
    // Function to draw the bullet
    void draw() const;

    glm::vec3 getPos() const { return position; };

    // Bullet sphere
    Sphere sphere;
    
private:
    glm::vec3 position;  // Bullet position
    glm::vec3 direction; // Direction of the bullet (camera front)
    float speed;         // Speed at which the bullet moves
                         //
    int explosion;
    
    float maxDistance;
    bool isDestroyed;
};

#endif

