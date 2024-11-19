#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include "utils/m_shader.h"

typedef enum {
  SPEED,
  RANGE,
  ACCURACY,
  HEALTH
} benefit_t;

class Collectible {
public:
    Collectible(float radius, glm::vec3 start_pos);
    void draw(Shader& shader);
    bool contains(glm::vec3 point) const { return glm::distance(glm::vec3(x,y,z), point) < 1.1f; };
    glm::vec3 getPos() const { return glm::vec3(x,y,z); };
    mutable bool gone = false;
    benefit_t collect();

private:
    void buildVertices();
    glm::vec3 getBenefitColor(benefit_t benefit);

    float radius;
    int sectorCount;
    int stackCount;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;
    float x;
    float y;
    float z;

    mutable int frames_lived = 0;
    int max_life = 10000;
    benefit_t benefit;

};

#endif // SPHERE_H

