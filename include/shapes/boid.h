#ifndef BOID_H
#define BOID_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <cmath>

#include "shapes/box.h"

class Boid {
public:
    Boid(float size, glm::vec3 start_pos);

    bool act(glm::vec3 goal_pos, std::vector<Box> obstacles);
    void draw() const;
    glm::vec3 getPos() const { return position; };
    std::vector<glm::vec3> directions_from_view_angle(float angle);
    void avoidObstacles(std::vector<Box> boxes);

private:
    float rayStepSize = 0.0001f;
    float rayMaxLength = 0.001f;

    float forceApplicationCoefficient = 0.75f;
    float speedIncreaseCoefficient = 0.00003f;

    float obstacleRepelForce = 7.0f;
    float obstacleRepelDecay = 8.0f;

    float goalAttraction = 1.0f;

    void buildVertices();
    void applyForce(glm::vec3 force_direction, float strength);
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);

    float speed = 0.0f;
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
