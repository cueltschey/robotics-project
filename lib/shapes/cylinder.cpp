#include "shapes/cylinder.h"
#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Cylinder::Cylinder(glm::vec3 start_pos, float radius_, float height_, int segments_) : position(start_pos),
  radius(radius_), height(height_), segments(segments_)
       {

    modelMatrix = glm::mat4(1.0f);
    buildVertices();
}

bool Cylinder::contains(glm::vec3 point) const {
  return glm::distance(point, position) < 0.1f;
}

glm::vec3 Cylinder::rotateVertex(const glm::vec3& vertex, const glm::vec3& direction) {
    // If the direction is valid (non-zero)
    if (glm::length(direction) > 0.0f) {
        glm::vec3 forward = glm::normalize(direction);  // Normalize the direction vector
        glm::vec3 up(0.0f, 1.0f, 0.0f);  // World up vector

        // Compute right and adjusted up vectors to create the correct orientation
        glm::vec3 right = glm::normalize(glm::cross(up, forward));  // Right vector
        glm::vec3 adjustedUp = glm::cross(forward, right);  // Adjusted up vector

        // Construct the rotation matrix to align with the movement direction
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        rotationMatrix[0] = glm::vec4(right, 0.0f);      // Right vector
        rotationMatrix[1] = glm::vec4(adjustedUp, 0.0f); // Adjusted up vector
        rotationMatrix[2] = glm::vec4(forward, 0.0f);    // Forward vector

        // Apply the 90-degree rotation by rotating around the right axis (for example)
        glm::mat4 rotation90 = glm::mat4(1.0f);

        // Rotate by 90 degrees around the right vector
        float angle = glm::radians(90.0f);  // 90 degrees in radians
        rotation90 = glm::rotate(rotation90, angle, right);  // Rotate by 90 degrees

        // Combine the 90-degree rotation with the original rotation
        glm::mat4 combinedRotation = rotation90 * rotationMatrix;

        // Apply the combined rotation to the vertex
        glm::vec4 rotatedVertex = combinedRotation * glm::vec4(vertex, 1.0f);
        return glm::vec3(rotatedVertex);
    }

    // Return the original vertex if no direction is provided
    return vertex;
}

void Cylinder::buildVertices() {
// Clear previous vertices and indices
    vertices.clear();
    indices.clear();

    // Step 1: Generate vertices for the base and top circles
    for (int i = 0; i < segments; ++i) {
        // Calculate angle for this segment
        float angle = 2.0f * M_PI * i / segments;

        // Base circle vertex
        glm::vec3 baseVertex(radius * cos(angle), 0.0f, radius * sin(angle));
        baseVertex = rotateVertex(baseVertex, direction) + position;
        vertices.push_back(baseVertex.x);
        vertices.push_back(baseVertex.y);
        vertices.push_back(baseVertex.z);

        // Top circle vertex
        glm::vec3 topVertex(radius * cos(angle), height, radius * sin(angle));
        topVertex = rotateVertex(topVertex, direction) + position;
        vertices.push_back(topVertex.x);
        vertices.push_back(topVertex.y);
        vertices.push_back(topVertex.z);
    }

    // Step 2: Add center vertices for base and top (used to create triangle fans for caps)
    glm::vec3 baseCenter = rotateVertex(glm::vec3(0.0f, 0.0f, 0.0f), direction) + position;
    glm::vec3 topCenter = rotateVertex(glm::vec3(0.0f, height, 0.0f), direction) + position;

    vertices.push_back(baseCenter.x);
    vertices.push_back(baseCenter.y);
    vertices.push_back(baseCenter.z);

    vertices.push_back(topCenter.x);
    vertices.push_back(topCenter.y);
    vertices.push_back(topCenter.z);

    // Indices for the base and top center vertices
    int baseCenterIndex = 2 * segments;    // Index of base center
    int topCenterIndex = baseCenterIndex + 1; // Index of top center

    // Step 3: Define indices for the base and top circles
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;

        // Base
        indices.push_back(baseCenterIndex);
        indices.push_back(i * 2);
        indices.push_back(next * 2);

        // Top
        indices.push_back(topCenterIndex);
        indices.push_back(i * 2 + 1);
        indices.push_back(next * 2 + 1);

        // Side faces (two triangles per face)
        indices.push_back(i * 2);           // Base vertex
        indices.push_back(i * 2 + 1);       // Top vertex
        indices.push_back(next * 2 + 1);    // Next top vertex

        indices.push_back(i * 2);           // Base vertex
        indices.push_back(next * 2 + 1);    // Next top vertex
        indices.push_back(next * 2);        // Next base vertex
    }
    // Generate OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}



void Cylinder::draw() const {
    glBindVertexArray(VAO);

    // Apply the model matrix to the shader program
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using indices
    glBindVertexArray(0);

}

void Cylinder::drawLine(glm::vec3 start, glm::vec3 end){
    glBegin(GL_LINES);
    glVertex3fv(glm::value_ptr(start));
    glVertex3fv(glm::value_ptr(end));
    glEnd();
}

