#ifndef CYLINDER_H
#define CYLINDER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <cmath>

#include "shapes/box.h"
#include "shapes/sphere.h"

#include "utils/m_shader.h"

class Cylinder {
public:
    Cylinder(glm::vec3 start_pos, float radius_, float height_, int segments_);

    void draw() const;
    glm::vec3 getPos() const { return position; };
    void avoidObstacles(std::vector<Box> boxes, std::vector<Cylinder> boids);

    void drawLine(glm::vec3 start, glm::vec3 end);
    void setDirection(glm::vec3 d) const { direction = d; };
    void setPosition(glm::vec3 p) {
      position = p;
      buildVertices();
    };

    bool contains(glm::vec3 point) const;

    glm::mat4 getModelMatrix() const { return modelMatrix; };

private:

    void buildVertices();
    void applyForce(glm::vec3 force_direction, float strength);
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);

    void calculateNormals(const std::vector<glm::vec3>& unrotatedVertices);

    float speed = 0.0f;

    mutable glm::vec3 position;
    glm::mat4 modelMatrix;
    mutable glm::vec3 direction;
    std::vector<glm::vec3> normals;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;

    float radius;
    float height;
    int segments;

};

#endif
