#ifndef SPACE_H
#define SPACE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Space {
public:
    GLuint texture;
    GLuint VAO, VBO, EBO; // OpenGL objects for the sphere
    float radius;         // Radius of the background sphere

    // Constructor
    Space(float radius, GLuint texture);

    // Function to render the sphere
    void render();

private:
    // Helper function to generate sphere data (vertices, normals, texture coords)
    void generateSphereData(int sectors, int stacks);
};

#endif

