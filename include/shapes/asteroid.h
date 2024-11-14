#ifndef ASTEROID_H
#define ASTEROID_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "shapes/box.h"
#include "utils/m_shader.h"

#include <glm/vec3.hpp>
#include <vector>
#include <GL/glew.h>

class Asteroid : public Obstacle {
public:
    Asteroid(float radius, glm::vec3 start_pos, float speed_);
    void draw(Shader& shader) const override;
    void updatePos(glm::vec3 next_pos);
    void setPosition(glm::vec3 pos);
    float getX() const override { return x; }
    float getY() const override { return y; }
    float getZ() const override { return z; }

    glm::vec3 getPos() const override { return glm::vec3(x, y, z); }

    float getMinX() const override { return x - radius; }
    float getMaxX() const override { return x + radius; }
    float getMinY() const override { return y - radius; }
    float getMaxY() const override { return y + radius; }
    float getMinZ() const override { return z - radius; }
    float getMaxZ() const override { return z + radius; }

    float getWidth() const override { return 2.0f * radius; }
    float getHeight() const override { return 2.0f * radius; }
    float getDepth() const override { return 2.0f * radius; }

    bool contains(const glm::vec3& point) const override {
      return glm::distance(point, glm::vec3(x,y,z)) < radius; 
    };

private:
    void buildVertices();
    void rebuildVertices();

    float radius;
    int sectorCount;
    int stackCount;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;
    float x;
    float y;
    float z;
    float speed;
};

#endif // SPHERE_H

