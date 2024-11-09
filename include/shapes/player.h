#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <cmath>

#include "shapes/box.h"
#include "shapes/boid.h"

class Player {
public:
    Player(float size, glm::vec3 start_pos);

    void draw() const;
    glm::vec3 getPos() const { return position; };
    void updatePos(glm::vec3 nextPoint);
    void shoot(std::vector<Boid> boids);

private:
    void buildVertices();
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);

    float speed = 1.5f;
    float size;
    glm::vec3 position;
    glm::mat4 modelMatrix;
    glm::vec3 direction;
    std::vector<glm::vec3> directions;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;
};

#endif
