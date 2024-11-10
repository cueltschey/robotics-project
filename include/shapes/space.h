#ifndef SPACE_H
#define SPACE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shapes/sphere.h"

class Space {
public:

    // Constructor
    Space(float radius_, int numStars_, glm::vec3 playerPosition);

    // Function to render the sphere
    void render();

private:

    void createStars(const glm::vec3& playerPosition);
    glm::vec3 randomStarPos(const glm::vec3& playerPosition, float minDistance, float maxDistance);
    std::vector<Sphere> stars;
    float radius;
    int numStars;
};

#endif

