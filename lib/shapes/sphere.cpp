#include "shapes/sphere.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

Sphere::Sphere(float radius, int sectorCount, int stackCount, float xPos, float yPos, float zPos)
    : radius(radius), sectorCount(sectorCount), stackCount(stackCount), x(xPos), y(yPos), z(zPos) {
    buildVertices();
}

void Sphere::setPosition(float xPos, float yPos, float zPos) {
    x = xPos;
    y = yPos;
    z = zPos;
    // Rebuild the vertices to reflect the new position
    rebuildVertices();
}

void Sphere::buildVertices() {
    float xy;
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    // Clear previous vertices and indices
    vertices.clear();
    indices.clear();

    // Generate vertices and normals
    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;      // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // Add (sectorCount+1) vertices per stack
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // Vertex position with applied offset (x, y, z)
            float vertexX = xy * cosf(sectorAngle) + x;
            float vertexY = xy * sinf(sectorAngle) + y;
            float vertexZ = z + z;

            vertices.push_back(vertexX);
            vertices.push_back(vertexY);
            vertices.push_back(vertexZ);

            // Normal vector (normalized position)
            vertices.push_back(xy * cosf(sectorAngle) / radius);
            vertices.push_back(xy * sinf(sectorAngle) / radius);
            vertices.push_back(z / radius);
        }
    }

    // Generate indices
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);             // beginning of current stack
        int k2 = k1 + sectorCount + 1;              // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // Element buffer object for indices

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); // Position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); // Normal

    glBindVertexArray(0);
}

void Sphere::rebuildVertices() {
    buildVertices(); // Rebuild the vertices based on the new position
}

void Sphere::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using indices
    glBindVertexArray(0);
}

