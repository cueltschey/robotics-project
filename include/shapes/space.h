#ifndef SPACE_H
#define SPACE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shapes/sphere.h"
#include "shapes/asteroid.h"
#include "utils/m_shader.h"

class Space {
public:

    // Constructor
    Space(float stars_radius_, float asteroids_radius_, int numStars_, int numAsteroids_, glm::vec3 playerPosition);

    // Function to render the sphere
    void render(Shader& lightShader, Shader& textureShader);

private:

    void createStars(const glm::vec3& playerPosition);
    glm::vec3 randomStarPos(const glm::vec3& playerPosition, float minDistance, float maxDistance);
    glm::vec3 randomAsteroidPos(const glm::vec3& playerPosition, float minDistance, float maxDistance); std::vector<Sphere> stars;
    std::vector<Asteroid> asteroids;
    float stars_radius;
    float asteroids_radius;
    int numStars;
    int numAsteroids;
};

#endif

