#include "shapes/sun.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>


Sun::Sun(float radius, glm::vec3 start_pos, float speed_)
    : radius(radius), x(start_pos[0]), y(start_pos[1]), z(start_pos[2]), speed(speed_) {
    
    sectorCount = std::max(18, static_cast<int>(radius * 10)); // Higher for larger radii
    stackCount = std::max(9, static_cast<int>(radius * 5));    // Proportionally lower than sectors

    buildVertices();
}

void Sun::updatePos(glm::vec3 next_pos) {
    glm::vec3 direction = glm::normalize(next_pos - glm::vec3(x, y, z));

    // Update position based on direction and speed
    glm::vec3 new_pos = glm::vec3(x, y, z) + direction * speed;
    x = new_pos[0];
    y = new_pos[1];
    z = new_pos[2];

    // Rebuild vertices to reflect the new position
    rebuildVertices();
}

void Sun::setPosition(glm::vec3 pos) {
    x = pos.x; y = pos.y; z = pos.z;
    rebuildVertices();
}

void Sun::buildVertices() {
    float xy; 
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    // Clear previous vertices and indices
    vertices.clear();
    indices.clear();

    // Generate vertices and normals
    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;          // from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);                 // r * cos(stackAngle)
        float localZ = radius * sinf(stackAngle);       // r * sin(stackAngle)

        // Add vertices per sector
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;               // from 0 to 2pi

            // Vertex positions with applied offset (x, y, z)
            float vertexX = xy * cosf(sectorAngle) + this->x;
            float vertexY = xy * sinf(sectorAngle) + this->y;
            float vertexZ = localZ + this->z;

            vertices.push_back(vertexX);
            vertices.push_back(vertexY);
            vertices.push_back(vertexZ);

            // Normal vector (normalized position)
            vertices.push_back(xy * cosf(sectorAngle) / radius);
            vertices.push_back(xy * sinf(sectorAngle) / radius);
            vertices.push_back(localZ / radius);
        }
    }

    // Generate indices for the triangles of each stack
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);                 // beginning of current stack
        int k2 = k1 + sectorCount + 1;                  // beginning of next stack

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

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Sun::rebuildVertices() {
    buildVertices(); // Rebuild the vertices based on the new position
}

void Sun::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using indices
    glBindVertexArray(0);
}

