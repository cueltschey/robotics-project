#include "shapes/player.h"
#include <algorithm>
#include "utils/m_shader.h"

#include <glm/gtc/type_ptr.hpp>

namespace std {
    template<>
    struct hash<std::tuple<int, int, int>> {
        size_t operator()(const std::tuple<int, int, int>& t) const {
            size_t h1 = std::hash<int>{}(std::get<0>(t));
            size_t h2 = std::hash<int>{}(std::get<1>(t));
            size_t h3 = std::hash<int>{}(std::get<2>(t));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

#define CELL_SIZE 2.0f


Player::Player(float size, glm::vec3 start_pos)
    : size(size), position(start_pos), direction(0.0f), 
    turret_sphere(size * 0.50f, start_pos, 0.0f), turret_barrel(start_pos, 0.01f, 1.0f, 5) {

    modelMatrix = glm::mat4(1.0f);
    buildVertices();
}

glm::vec3 Player::rotateVertex(const glm::vec3& vertex, const glm::vec3& direction) {
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

void Player::buildVertices() {
    float halfSize = size / 2.0f;

    // Define the base vertices (before rotation)
    std::vector<glm::vec3> unrotatedVertices = {
        {-halfSize, 0.0f, -halfSize},   // Base - bottom left
        { halfSize, 0.0f, -halfSize},   // Base - bottom right
        { halfSize, 0.0f,  halfSize},   // Base - top right
        {-halfSize, 0.0f,  halfSize},   // Base - top left
        { 0.0f, size,  0.0f}            // Apex
    };

    // Clear previous vertices
    vertices.clear();

    // Rotate and translate the vertices, and push them to the vertices vector
    for (const auto& vertex : unrotatedVertices) {
        glm::vec3 rotatedVertex = rotateVertex(vertex, direction);  // Apply rotation
        rotatedVertex += position; // Translate the vertex based on boid's position
        
        // Add the rotated and translated vertex to the vertices vector
        vertices.push_back(rotatedVertex.x);
        vertices.push_back(rotatedVertex.y);
        vertices.push_back(rotatedVertex.z);
    }

    // Define indices for the base and sides (unchanged)
    indices = {
        0, 1, 2,    // Base face
        0, 2, 3,
        0, 1, 4,    // Side faces
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

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



void Player::draw(
    std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map,
    Shader& shader) {
    shader.use();
    if(trail.size() >= 40){
      trail.erase(trail.begin());
    }
    trail.push_back(Sphere(0.01f, position, 0.0f));

    shader.setVec3("objectColor", glm::vec3(0.5f,0.5f,0.5f));
    turret_barrel.draw();

    glBindVertexArray(VAO);

    // Apply the model matrix to the shader program
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using indices
    glBindVertexArray(0);


    shader.setVec3("objectColor", glm::vec3(0.0f,1.0f,0.0f));
    turret_sphere.draw();

    shader.setVec3("objectColor", glm::vec3(1.0f,0.5f,0.0f));
    for(Sphere s : trail){
      s.draw();
    }


    for(size_t i = 0; i < bullets.size(); i++){
      if(bullets[i].gone){
        bullets.erase(bullets.begin() + i);
        continue;
      }
      Bullet b = bullets[i];
      shader.setVec3("objectColor", glm::vec3(1.0f - b.colorFade,0.5f - b.colorFade,0.0f));
      std::vector<Boid> boids = boid_map[positionToCell(b.getPos())];
      b.draw();
    }
}

std::tuple<int, int, int> Player::positionToCell(const glm::vec3& pos) {
    int cellX = static_cast<int>(std::ceil(pos.x) / CELL_SIZE);
    int cellY = static_cast<int>(std::ceil(pos.y) / CELL_SIZE);
    int cellZ = static_cast<int>(std::ceil(pos.z) / CELL_SIZE);

    return std::make_tuple(cellX, cellY, cellZ);
}

void Player::updatePos(glm::vec3 cameraFront) {
    position += direction * std::min(speed, 10.1f);
    turret_sphere.setPosition(position + direction * 0.05f);
    turret_barrel.setDirection(cameraFront + glm::vec3(0.0f, 0.2f, 0.0f));
    turret_barrel.setPosition(position + direction * 0.05f);
    buildVertices();
    since_last_shot--;
}

void Player::applyForce(glm::vec3 force_direction, float strength){
    glm::vec3 normalized_force = glm::normalize(force_direction);
    glm::vec3 force = normalized_force * strength;
    direction += force * 1.02f;
    direction = glm::normalize(direction);

    float force_magnitude = glm::length(force);
    speed *= 0.95;
    speed += force_magnitude * 0.1f;
    speed = glm::clamp(speed, 0.0f, 1.0f);
    drawLine(position,  position + normalized_force * strength * 10.0f);
}

void Player::drawLine(glm::vec3 start, glm::vec3 end){
    glBegin(GL_LINES);
    glVertex3fv(glm::value_ptr(start));
    glVertex3fv(glm::value_ptr(end));
    glEnd();
}

void Player::shoot(glm::vec3 cameraFront,
    std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map){
    if(since_last_shot <= 0){
      bullets.push_back(Bullet(position, cameraFront, boid_map));
    }
}

