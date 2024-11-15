#include "shapes/player.h"
#include <algorithm>
#include "utils/m_shader.h"

#include <glm/gtc/type_ptr.hpp>
#include "utils/generation.h"



Player::Player(float size, glm::vec3 start_pos)
    : size(size), position(start_pos), direction(0.0f), 
    thruster(size * 0.25, start_pos, 0.0f), aimer(0.05f, start_pos, 0.0f) {

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
    Shader& shader, int frames_since_shot, int shot_cooldown) {
    shader.use();

    shader.setVec3("objectColor", glm::vec3(
          1.0f - (frames_since_shot / shot_cooldown),
          (frames_since_shot / shot_cooldown),0.0f));
    aimer.draw();

    shader.setVec3("objectColor", glm::vec3(0.2f,0.2f,0.2f));

    glBindVertexArray(VAO);

    // Apply the model matrix to the shader program
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using indices
    glBindVertexArray(0);


    shader.setVec3("objectColor", glm::vec3(0.5f, 0.25f + speed * 3.00f, 0.25f + speed * 3.00f));
    thruster.draw();

    shader.setVec3("objectColor", glm::vec3(1.0f,0.5f,0.0f));


    for(size_t i = 0; i < bullets.size(); i++){
      if(bullets[i].gone){
        bullets.erase(bullets.begin() + i);
        continue;
      }
      Bullet& b = bullets[i];
      shader.setVec3("objectColor", glm::vec3(1.0f - b.colorFade,1.0f - b.colorFade,0.0f));
      std::vector<Boid> boids = boid_map[positionToCell(b.getPos())];
      b.draw(shader);
    }
}


void Player::updatePos(glm::vec3 cameraFront) {
    if (isOrbiting) {
        position = orbitPlanetPos + toOrbitPlanet * orbitRange;

        position += direction * std::min(speed, maxSpeed);

        // update planet reference variables
        orbitRange = glm::distance(orbitPlanetPos, position);
        toOrbitPlanet = glm::normalize(position - orbitPlanetPos);
        speed *= 0.90;
        if(orbitRange >= orbitPlanetGravity){
          isOrbiting = false;
        }

    } else {
        position += direction * std::min(speed, maxSpeed);
    }
    thruster.setPosition(position);
    aimer.setPosition(glm::mix(aimer.getPos(),
          position + glm::normalize(cameraFront + glm::vec3(0.0f,0.2f,0.0f)) * 20.0f, 0.3f));
    buildVertices();
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
}

void Player::drawLine(glm::vec3 start, glm::vec3 end){
    glBegin(GL_LINES);
    glVertex3fv(glm::value_ptr(start));
    glVertex3fv(glm::value_ptr(end));
    glEnd();
}

void Player::shoot(std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map){
    bullets.push_back(Bullet(position, glm::normalize(aimer.getPos() - position), boid_map));
}

void Player::requestOrbit(glm::vec3 planetPos, float orbitThreshold) {
    float range = glm::distance(planetPos, position);

    if (range < orbitThreshold) {
        if(!isOrbiting){
          orbitPlanetGravity = orbitThreshold;
          orbitRange = range;
          toOrbitPlanet = glm::normalize(position - planetPos);
          isOrbiting = true;
        }
        orbitPlanetPos = planetPos;
    }
}
