#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Planet {
public:
    Planet(float radius, glm::vec3 start_pos);
    void draw() const;
    void updatePos(glm::vec3 parentPos);
    void setPosition(glm::vec3 pos);
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    glm::vec3 getPos() const { return glm::vec3(x,y,z); };
    void orbit(float orbit_radius_, float speed_);


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
    bool isOrbiting = false;
    float orbit_radius;
    float orbit_angle;
};

#endif // SPHERE_H

