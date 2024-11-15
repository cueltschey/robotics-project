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


class Boid {
public:
    Boid(long int frame, glm::vec3 start_pos);

    bool act(glm::vec3 goal_pos, std::vector<Obstacle*> obstacles, glm::vec3 flock_center, std::vector<Boid>& boids);
    void draw(Shader& shader) const;
    glm::vec3 getPos() const { return position; };
    std::vector<glm::vec3> directions_from_view_angle(float angle);
    void avoidObstacles(std::vector<Obstacle*> boxes, std::vector<Boid> boids);

    void drawLine(glm::vec3 start, glm::vec3 end);

    bool contains(glm::vec3 point) const;

    void explode() {
      dead = true;
    };

    glm::vec3 getDirection() const { return direction; };
    void applyFlockForces(std::vector<Boid>& boids);

private:

    void buildVertices();
    void applyForce(glm::vec3 force_direction, float strength);
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);

    void calculateNormals(const std::vector<glm::vec3>& unrotatedVertices);

    bool dead = false;

    float speed = 0.0f;

    int trailLength = 5;
    std::vector<glm::vec3> directions;
    std::vector<glm::vec3> normals;


    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;

    // Colors
    float boidR, boidG, boidB;

    // Ray parameters
    float rayStepSize = 0.0001f;
    float rayMaxLength = 0.001f;

    // Force coefficients
    float forceApplicationCoefficient = 0.75f;
    float speedIncreaseCoefficient = 0.002f;

    // Obstacle avoidance parameters
    float obstacleRepelForce = 7.0f;
    float obstacleRepelDecay = 8.0f;
    float boidRepelForce = 2.0f;
    float boidRepelDecay = 15.0f;

    // Goal attraction
    float goalAttraction = 1.2f;
    float flockAttraction = 1.0f; //0.7f;

    float size = 0.1f;

    float maxDetectionRange = 30.0f;
    float maxBoidSpeed = 1.0f;

    glm::vec3 direction;
    glm::vec3 position;

};

#endif
