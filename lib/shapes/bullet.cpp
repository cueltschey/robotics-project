#include "shapes/bullet.h"
#include <glm/gtc/matrix_transform.hpp>

Bullet::Bullet(glm::vec3 startPos, glm::vec3 cameraFront, float speed, int explosion_)
    : position(startPos), 
    direction(glm::normalize(cameraFront)), 
    speed(speed),
    maxDistance(50.0f),
    isDestroyed(false),
    sphere(0.01f, position, speed), explosion(explosion_) {
}

void Bullet::updatePosition(glm::vec3 playerPos) {
    // Move the bullet in the direction of the camera front
    if (!isDestroyed) {
        glm::vec3 movement = direction * speed;
        glm::vec3 next_pos = position + movement;
        sphere.updatePos(next_pos);

        position = next_pos;

        // Check if the bullet has exceeded its max travel distance
        if (glm::length(position - playerPos) > maxDistance) {
            isDestroyed = true;  // Mark the bullet for destruction
        }
    }
}

void Bullet::checkCollisions(std::vector<Boid> boids) {
    if(isDestroyed)
      return;
    for (Boid& boid : boids) {
        glm::vec3 boidPos = boid.getPos();

        // Simple distance check for collision
        float distance = glm::length(boidPos - position);
        if (distance < 0.1f) {
            //playSound(explosion);
            boid.explode();  // Destroy the boid if collision occurs
            isDestroyed = true;  // Mark the bullet as destroyed
            break;  // Stop checking for collisions after one hit
        }
    }
}

void Bullet::draw() const {
    if (!isDestroyed) {
        // Draw the sphere representing the bullet
        sphere.draw();
    }
}

