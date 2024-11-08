#include "shapes/box.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

Box::Box(float depth_, float width_, float height_, float xPos, float yPos, float zPos)
    : depth(depth_), width(width_), height(height_), x(xPos), y(yPos), z(zPos) {
    buildVertices();
}

void Box::setPosition(float xPos, float yPos, float zPos) {
    x = xPos;
    y = yPos;
    z = zPos;
    rebuildVertices();
}

void Box::buildVertices() {
    vertices.clear();
    indices.clear();

    // Vertices of a box (positions only)
    vertices = {
        // Positions of each vertex (x, y, z)
        -width / 2 + x, -height / 2 + y, -depth / 2 + z, // Bottom-left-front
        width / 2 + x, -height / 2 + y, -depth / 2 + z,  // Bottom-right-front
        width / 2 + x, height / 2 + y, -depth / 2 + z,   // Top-right-front
        -width / 2 + x, height / 2 + y, -depth / 2 + z,  // Top-left-front
        -width / 2 + x, -height / 2 + y, depth / 2 + z,  // Bottom-left-back
        width / 2 + x, -height / 2 + y, depth / 2 + z,   // Bottom-right-back
        width / 2 + x, height / 2 + y, depth / 2 + z,    // Top-right-back
        -width / 2 + x, height / 2 + y, depth / 2 + z,   // Top-left-back
    };

    indices = {
        0, 1, 2,  2, 3, 0,   // Front face
        4, 5, 6,  6, 7, 4,   // Back face
        0, 3, 7,  7, 4, 0,   // Left face
        1, 5, 6,  6, 2, 1,   // Right face
        0, 1, 5,  5, 4, 0,   // Bottom face
        3, 2, 6,  6, 7, 3    // Top face
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Only define the vertex attribute for positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Box::rebuildVertices() {
    buildVertices(); // Rebuild the vertices with the new position and color
}

// This function draws the box using OpenGL.
void Box::draw() const {
    // Bind the VAO (Vertex Array Object)
    glBindVertexArray(VAO);

    // Draw the box (using indices)
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
}

