#ifndef BOID_H
#define BOID_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <cmath>

#include "shapes/box.h"
#include "shapes/sphere.h"

#include "utils/m_shader.h"

struct BoidParams {
    // Colors
    float boidR, boidG, boidB;
    float trailR, trailG, trailB;
    

    // Ray parameters
    float rayStepSize = 0.0001f;
    float rayMaxLength = 0.001f;

    // Force coefficients
    float forceApplicationCoefficient = 0.75f;
    float speedIncreaseCoefficient = 0.00003f;

    // Obstacle avoidance parameters
    float obstacleRepelForce = 7.0f;
    float obstacleRepelDecay = 8.0f;

    // Goal attraction
    float goalAttraction = 1.0f;
    float maxBoidSpeed = 1.0f;
    float size = 0.1f;
};

class Boid {
public:
    Boid(glm::vec3 start_pos, const BoidParams& params);

    bool act(glm::vec3 goal_pos, std::vector<Box> obstacles, glm::vec3 flock_center, std::vector<Boid>& boids);
    void draw(Shader& shader) const;
    glm::vec3 getPos() const { return position; };
    std::vector<glm::vec3> directions_from_view_angle(float angle);
    void avoidObstacles(std::vector<Box> boxes, std::vector<Boid> boids);

    void drawLine(glm::vec3 start, glm::vec3 end);

    bool contains(glm::vec3 point) const;

    glm::mat4 getModelMatrix() const { return modelMatrix; };
    void explode() {
      dead = true;
    };

    glm::vec3 getDirection() const { return direction; };
    void applyFlockForces(std::vector<Boid>& boids);

private:

    float boidR, boidG, boidB;
    float trailR, trailG, trailB;

    float rayStepSize = 0.0001f;
    float rayMaxLength = 0.001f;

    float forceApplicationCoefficient = 0.75f;
    float speedIncreaseCoefficient = 0.00003f;

    float obstacleRepelForce = 7.0f;
    float obstacleRepelDecay = 8.0f;
    float boidRepelForce = 2.0f;
    float boidRepelDecay = 15.0f;

    float goalAttraction = 1.0f;
    float flockAttraction = 0.7f;

    float maxBoidSpeed = 1.0f;
    float maxDetectionRange = 30.0f;

    void buildVertices();
    void applyForce(glm::vec3 force_direction, float strength);
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);

    void calculateNormals(const std::vector<glm::vec3>& unrotatedVertices);

    bool dead = false;

    float speed = 0.0f;

    int trailLength = 5;
    float size;
    glm::vec3 position;
    glm::mat4 modelMatrix;
    glm::vec3 direction;
    std::vector<glm::vec3> directions;
    std::vector<glm::vec3> normals;


    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;

};

#endif
