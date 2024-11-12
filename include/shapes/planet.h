#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <optional>
#include <algorithm>

class Planet {
public:
    Planet(float radius, glm::vec3 start_pos, float gravity_ = 0.0f)
        : radius(radius), position(start_pos), gravity(gravity_) {
        sectorCount = std::max(18, static_cast<int>(radius * 10)); // Higher for larger radii
        stackCount = std::max(9, static_cast<int>(radius * 5));    // Proportionally lower than sectors

        buildVertices();
    }
    void draw() const;
    void updatePos(glm::vec3 parentPos);
    void setPosition(glm::vec3 pos);
    float getX() const { return position.x; }
    float getY() const { return position.y; }
    float getZ() const { return position.z; }
    glm::vec3 getPos() const { return position; };
    void orbit(float orbit_radius_, float speed_);

    bool contains(glm::vec3 p) const { return glm::distance(position, p) < radius; };
    float radius;
    float gravity;


private:
    void buildVertices();
    void rebuildVertices();

    glm::vec3 position;
    int sectorCount;
    int stackCount;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;
    float speed;
    bool isOrbiting = false;
    float orbit_radius;
    float orbit_angle;
};

#endif // SPHERE_H

