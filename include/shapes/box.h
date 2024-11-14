#ifndef BOX_H
#define BOX_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "utils/m_shader.h"


class Obstacle {
public:
    virtual ~Obstacle() = default;

    // Mark all getters as virtual with the 'const' qualifier to allow overriding
    virtual float getMinX() const { return 0.0f; }
    virtual float getMaxX() const { return 0.0f; }
    virtual float getMinY() const { return 0.0f; }
    virtual float getMaxY() const { return 0.0f; }
    virtual float getMinZ() const { return 0.0f; }
    virtual float getMaxZ() const { return 0.0f; }

    virtual float getX() const { return 0.0f; }
    virtual float getY() const { return 0.0f; }
    virtual float getZ() const { return 0.0f; }

    // Mark width, height, depth, and contains as virtual
    virtual float getWidth() const { return 0.0f; }
    virtual float getHeight() const { return 0.0f; }
    virtual float getDepth() const { return 0.0f; }

    virtual bool contains(const glm::vec3& point) const { return false; }

    virtual void draw(Shader& shader) const { };
    virtual glm::vec3 getPos() const { return glm::vec3(0.0f); };
};



class Box : public Obstacle {
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
    void draw(Shader& shader) const override;
    void buildVertices();
    void setPosition(float xPos, float yPos, float zPos);
    void rebuildVertices();
    float getMinX() const override {
        return x - width / 2;
    }

    float getMaxX() const override {
        return x + width / 2;
    }

    float getMinY() const override {
        return y - height / 2;
    }

    float getMaxY() const override {
        return y + height / 2;
    }

    float getMinZ() const override {
        return z - depth / 2;
    }

    float getMaxZ() const override {
        return z + depth / 2;
    }

    float getX() const override { return x; }
    float getY() const override { return y; }
    float getZ() const override { return z; }
    float getWidth() const override { return width; }
    float getHeight() const override { return height; }
    float getDepth() const override { return depth; }

    bool contains(const glm::vec3& point) const override {
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

