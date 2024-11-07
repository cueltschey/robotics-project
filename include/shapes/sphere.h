#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Sphere {
public:
    Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
    void draw() const;

private:
    void buildVertices();

    float radius;
    int sectorCount;
    int stackCount;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};

#endif // SPHERE_H

