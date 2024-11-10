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

    // Vertices and normals for each vertex (positions + normals)
    vertices = {
        // Positions of each vertex (x, y, z) and their normals (nx, ny, nz)
        // Front face
        -width / 2 + x, -height / 2 + y, -depth / 2 + z, 0.0f, 0.0f, -1.0f, // Bottom-left-front
        width / 2 + x, -height / 2 + y, -depth / 2 + z, 0.0f, 0.0f, -1.0f,  // Bottom-right-front
        width / 2 + x, height / 2 + y, -depth / 2 + z, 0.0f, 0.0f, -1.0f,   // Top-right-front
        -width / 2 + x, height / 2 + y, -depth / 2 + z, 0.0f, 0.0f, -1.0f,  // Top-left-front

        // Back face
        -width / 2 + x, -height / 2 + y, depth / 2 + z, 0.0f, 0.0f, 1.0f,  // Bottom-left-back
        width / 2 + x, -height / 2 + y, depth / 2 + z, 0.0f, 0.0f, 1.0f,   // Bottom-right-back
        width / 2 + x, height / 2 + y, depth / 2 + z, 0.0f, 0.0f, 1.0f,    // Top-right-back
        -width / 2 + x, height / 2 + y, depth / 2 + z, 0.0f, 0.0f, 1.0f,   // Top-left-back

        // Left face
        -width / 2 + x, -height / 2 + y, -depth / 2 + z, -1.0f, 0.0f, 0.0f, // Bottom-left-front
        -width / 2 + x, height / 2 + y, -depth / 2 + z, -1.0f, 0.0f, 0.0f,  // Top-left-front
        -width / 2 + x, height / 2 + y, depth / 2 + z, -1.0f, 0.0f, 0.0f,   // Top-left-back
        -width / 2 + x, -height / 2 + y, depth / 2 + z, -1.0f, 0.0f, 0.0f,  // Bottom-left-back

        // Right face
        width / 2 + x, -height / 2 + y, -depth / 2 + z, 1.0f, 0.0f, 0.0f,  // Bottom-right-front
        width / 2 + x, height / 2 + y, -depth / 2 + z, 1.0f, 0.0f, 0.0f,   // Top-right-front
        width / 2 + x, height / 2 + y, depth / 2 + z, 1.0f, 0.0f, 0.0f,    // Top-right-back
        width / 2 + x, -height / 2 + y, depth / 2 + z, 1.0f, 0.0f, 0.0f,   // Bottom-right-back

        // Bottom face
        -width / 2 + x, -height / 2 + y, -depth / 2 + z, 0.0f, -1.0f, 0.0f, // Bottom-left-front
        width / 2 + x, -height / 2 + y, -depth / 2 + z, 0.0f, -1.0f, 0.0f,  // Bottom-right-front
        width / 2 + x, -height / 2 + y, depth / 2 + z, 0.0f, -1.0f, 0.0f,   // Bottom-right-back
        -width / 2 + x, -height / 2 + y, depth / 2 + z, 0.0f, -1.0f, 0.0f,  // Bottom-left-back

        // Top face
        -width / 2 + x, height / 2 + y, -depth / 2 + z, 0.0f, 1.0f, 0.0f,  // Top-left-front
        width / 2 + x, height / 2 + y, -depth / 2 + z, 0.0f, 1.0f, 0.0f,   // Top-right-front
        width / 2 + x, height / 2 + y, depth / 2 + z, 0.0f, 1.0f, 0.0f,    // Top-right-back
        -width / 2 + x, height / 2 + y, depth / 2 + z, 0.0f, 1.0f, 0.0f    // Top-left-back
    };

    indices = {
        0, 1, 2,  2, 3, 0,   // Front face
        4, 5, 6,  6, 7, 4,   // Back face
        8, 9, 10, 10, 11, 8,  // Left face
        12, 13, 14, 14, 15, 12, // Right face
        16, 17, 18, 18, 19, 16,  // Bottom face
        20, 21, 22, 22, 23, 20   // Top face
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Positions attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

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

