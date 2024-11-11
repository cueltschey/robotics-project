#include "shapes/planet.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>


void Planet::orbit(float orbit_radius_, float speed_){
  speed = speed_;
  isOrbiting = true;
  orbit_radius = orbit_radius_;
  orbit_angle = 0.0f;
}

void Planet::updatePos(glm::vec3 parentPos) {
    if(isOrbiting){
      orbit_angle += speed;
      if (orbit_angle >= 360.0f) {  // Reset the angle to stay within 0-360 degrees
          orbit_angle -= 360.0f;
      }

      float centerX = parentPos.x;
      float centerY = parentPos.y;
      float centerZ = parentPos.z;

      // Assuming a 2D circular orbit in the xy-plane for simplicity
      position.x = centerX + orbit_radius * cos(glm::radians(orbit_angle));
      position.y = centerY + orbit_radius * sin(glm::radians(orbit_angle));
      position.z = centerZ;  // If orbiting in the xy-plane, z remains the same
    }
    // Rebuild vertices to reflect the new position
    rebuildVertices();
}

void Planet::setPosition(glm::vec3 pos) {
    position = pos;
    rebuildVertices();
}

void Planet::buildVertices() {
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
            float vertexX = xy * cosf(sectorAngle) + position.x;
            float vertexY = xy * sinf(sectorAngle) + position.y;
            float vertexZ = localZ + position.z;

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

void Planet::rebuildVertices() {
    buildVertices(); // Rebuild the vertices based on the new position
}

void Planet::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using indices
    glBindVertexArray(0);
}

