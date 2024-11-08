#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Sphere {
public:
    Sphere(float radius, int sectorCount, int stackCount, float xPos, float yPos, float zPos);
    void draw() const;
    void setPosition(float xPos, float yPos, float zPos);
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }


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
};

#endif // SPHERE_H

