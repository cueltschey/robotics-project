#include "shapes/space.h"
#include <iostream>

Space::Space(float radius, GLuint texture_)
    : radius(radius), texture(texture_) {
    generateSphereData(50, 50);  // 50 sectors and 50 stacks for a smooth sphere
    }


// Generate sphere data (vertices, normals, texture coordinates)
void Space::generateSphereData(int sectors, int stacks) {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    float x, y, z, xy;                       // Vertex position
    float u, v;                               // Texture coordinates

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;     // Starting from the top of the sphere
        xy = radius * cosf(stackAngle);            // xy is the radius at the current stack
        z = radius * sinf(stackAngle);             // z is the height at the current stack

        // Generate the vertices for each sector in this stack
        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;  // Angle along the stack (around the sphere)
            x = xy * cosf(sectorAngle);    // x = xy * cos(theta)
            y = xy * sinf(sectorAngle);    // y = xy * sin(theta)
            u = (float)j / sectors;        // Texture coordinate u
            v = (float)i / stacks;         // Texture coordinate v

            // Push the vertex data
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // Generate indices
    GLuint k1, k2;
    for (int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1);         // Current stack
        k2 = k1 + sectors + 1;          // Next stack

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    // Generate buffers and link data
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // Unbind the VAO
}


// Render the background sphere
void Space::render() {
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * 50 * 50, GL_UNSIGNED_INT, 0);  // 6 indices per quad
    glBindVertexArray(0);
}

