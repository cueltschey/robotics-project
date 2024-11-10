#ifndef BOX_H
#define BOX_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "utils/m_shader.h"


class Box {
public:
    Box(float depth_,
        float width_,
        float height_,
        float xPos,
        float yPos,
        float zPos,
        float r_,
        float g_,
        float b_);
    void draw(Shader& shader) const;
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

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getDepth() const { return depth; }

    bool contains(const glm::vec3& point) const {
        return point.x >= getMinX() && point.x <= getMaxX() &&
               point.y >= getMinY() && point.y <= getMaxY() &&
               point.z >= getMinZ() && point.z <= getMaxZ();
    }



private:
    float bottom_left;
    float depth;
    float width;
    float height;
    float x, y, z;
    float r, g, b;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;
};

#endif

