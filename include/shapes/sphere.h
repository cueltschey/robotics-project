#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Sphere {
public:
    Sphere(float radius, glm::vec3 start_pos, float speed_);
    void draw() const;
    void updatePos(glm::vec3 next_pos);
    void setPosition(glm::vec3 pos);
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

    glm::vec3 getPos() const { return glm::vec3(x,y,z); };


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

