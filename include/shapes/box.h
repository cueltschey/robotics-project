#ifndef BOX_H
#define BOX_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>


class Box {
public:
    Box(float depth_, float width_, float height_, float xPos, float yPos, float zPos);
    void draw() const;
    void buildVertices();
    void setPosition(float xPos, float yPos, float zPos);
    void rebuildVertices();
    float getMinX() const {
        return x - width / 2;
    }

    float getMaxX() const {
        return x + width / 2;
    }

    float getMinY() const {
        return y - height / 2;
    }

    float getMaxY() const {
        return y + height / 2;
    }

    float getMinZ() const {
        return z - depth / 2;
    }

    float getMaxZ() const {
        return z + depth / 2;
    }




private:
    float bottom_left;
    float depth;
    float width;
    float height;
    float x, y, z;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;
};

#endif

