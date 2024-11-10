#include "shapes/boid.h"
#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Boid::Boid(glm::vec3 start_pos, const BoidParams& params)
    : size(params.size), position(start_pos), direction(0.0f, 0.0f, 0.0f),
      boidR(params.boidR), boidG(params.boidG), boidB(params.boidB),
      trailR(params.trailR), trailG(params.trailG), trailB(params.trailB),
      rayStepSize(params.rayStepSize), rayMaxLength(params.rayMaxLength),
      forceApplicationCoefficient(params.forceApplicationCoefficient),
      speedIncreaseCoefficient(params.speedIncreaseCoefficient),
      obstacleRepelForce(params.obstacleRepelForce),
      obstacleRepelDecay(params.obstacleRepelDecay),
      goalAttraction(params.goalAttraction) {

    modelMatrix = glm::mat4(1.0f);
    directions = directions_from_view_angle(120.0f);
    buildVertices();
}

bool Boid::contains(glm::vec3 point) const {
  return glm::distance(point, position) < 0.1f;
}

glm::vec3 Boid::rotateVertex(const glm::vec3& vertex, const glm::vec3& direction) {
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

void Boid::buildVertices() {
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

bool Boid::act(glm::vec3 goal_pos, std::vector<Box> obstacles, glm::vec3 flock_center, std::vector<Boid> boids) {
    if(trail.size() >= 10){
      trail.erase(trail.begin());
    }
    if(frame % 4 == 0){
      trail.push_back(Sphere(0.01f, position, 0.0f));
    }
    frame++;
    if (dead) {
      return false;
    }
    auto collisionBox = std::find_if(obstacles.begin(), obstacles.end(), [&](const Box& box) {
        return box.contains(position);
    });
    if(collisionBox != obstacles.end()){
      return false;
    }

    glm::vec3 flock_center_direction = glm::normalize(flock_center - position);
    applyForce(flock_center_direction, flockAttraction);

    glm::vec3 new_direction = glm::normalize(goal_pos - position);
    applyForce(new_direction, goalAttraction);
    position += direction * speed;
    speed *= 0.92;
    avoidObstacles(obstacles, boids);
    buildVertices();
    return true;
}

void Boid::applyForce(glm::vec3 force_direction, float strength) {
    glm::vec3 normalized_force = glm::normalize(force_direction);
    glm::vec3 force = normalized_force * strength;
    direction += force * forceApplicationCoefficient;
    direction = glm::normalize(direction);

    float force_magnitude = glm::length(force);
    speed += force_magnitude * speedIncreaseCoefficient;
    speed = glm::clamp(speed, 0.0f, maxBoidSpeed);
}

void Boid::draw(Shader& shader) const {
    shader.setVec3("objectColor", boidR, boidG, boidB);


    glBindVertexArray(VAO);

    // Apply the model matrix to the shader program
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draw using indices
    glBindVertexArray(0);

    shader.setVec3("objectColor", trailR, trailG, trailB);
    for(Sphere s : trail){
      s.draw();
    }
}


std::vector<glm::vec3> Boid::directions_from_view_angle(float angle){
    std::vector<glm::vec3> directions = {
        glm::vec3(1.0f, 0.0f, 0.0f),  // Right
        glm::vec3(-1.0f, 0.0f, 0.0f), // Left
        glm::vec3(0.0f, 1.0f, 0.0f),  // Up
        glm::vec3(0.0f, -1.0f, 0.0f), // Down
        glm::vec3(0.0f, 0.0f, 1.0f),  // Forward
        glm::vec3(0.0f, 0.0f, -1.0f), // Backward

        // Diagonal directions
        glm::vec3(1.0f, 1.0f, 0.0f),  // Right-Up
        glm::vec3(1.0f, -1.0f, 0.0f), // Right-Down
        glm::vec3(-1.0f, 1.0f, 0.0f), // Left-Up
        glm::vec3(-1.0f, -1.0f, 0.0f),// Left-Down

        glm::vec3(1.0f, 0.0f, 1.0f),  // Right-Forward
        glm::vec3(1.0f, 0.0f, -1.0f), // Right-Backward
        glm::vec3(-1.0f, 0.0f, 1.0f), // Left-Forward
        glm::vec3(-1.0f, 0.0f, -1.0f),// Left-Backward

        glm::vec3(0.0f, 1.0f, 1.0f),  // Up-Forward
        glm::vec3(0.0f, 1.0f, -1.0f), // Up-Backward
        glm::vec3(0.0f, -1.0f, 1.0f), // Down-Forward
        glm::vec3(0.0f, -1.0f, -1.0f),// Down-Backward

        // 3D diagonals (combinations of X, Y, and Z)
        glm::vec3(1.0f, 1.0f, 1.0f),  // Right-Up-Forward
        glm::vec3(1.0f, 1.0f, -1.0f), // Right-Up-Backward
        glm::vec3(1.0f, -1.0f, 1.0f), // Right-Down-Forward
        glm::vec3(1.0f, -1.0f, -1.0f),// Right-Down-Backward

        glm::vec3(-1.0f, 1.0f, 1.0f), // Left-Up-Forward
        glm::vec3(-1.0f, 1.0f, -1.0f),// Left-Up-Backward
        glm::vec3(-1.0f, -1.0f, 1.0f),// Left-Down-Forward
        glm::vec3(-1.0f, -1.0f, -1.0f)// Left-Down-Backward
    };
    std::vector<glm::vec3> kept_dirs;

    // Forward direction
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

    float cos120 = std::cos(glm::radians(angle));

    for (auto& direction : directions) {
        float dotProduct = glm::dot(glm::normalize(forward), glm::normalize(direction));
        if (!dotProduct < cos120) {
            kept_dirs.push_back(direction);
        }
    }
    return kept_dirs;
}

void Boid::avoidObstacles(std::vector<Box> boxes, std::vector<Boid> boids){

  for(auto dir : directions){
    glm::vec3 rotatedDir = glm::normalize(direction - dir);
    glm::vec3 rayPosition = position;

    for (float distance = 0.0f; distance <= rayMaxLength; distance += rayStepSize) {
        rayPosition += rotatedDir;

        auto collisionBox = std::find_if(boxes.begin(), boxes.end(), [&](const Box& box) {
            return box.contains(rayPosition);
        });

        if (collisionBox != boxes.end()) {
            applyForce(- glm::normalize(rayPosition - position) 
                ,obstacleRepelForce / (glm::distance(rayPosition, position) * obstacleRepelDecay));
            drawLine(rayPosition, position);
            break;
        }


    }
  }

  for(Boid& boid : boids){
    applyForce(- glm::normalize(boid.getPos() - position) 
        ,boidRepelForce / (glm::distance(boid.getPos(), position) * boidRepelDecay));
    //drawLine(position, boid.getPos());
  }
}

void Boid::drawLine(glm::vec3 start, glm::vec3 end){
    glBegin(GL_LINES);
    glVertex3fv(glm::value_ptr(start));
    glVertex3fv(glm::value_ptr(end));
    glEnd();
}

