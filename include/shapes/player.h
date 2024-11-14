#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <cmath>
#include <unordered_map>

#include "shapes/box.h"
#include "shapes/boid.h"
#include "utils/m_shader.h"
#include "shapes/sphere.h"
#include "shapes/cylinder.h"
#include "shapes/bullet.h"
#include <tuple>
#include <unordered_map>

class Player {
public:
    Player(float size, glm::vec3 start_pos);

    void draw(
      std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map,
      Shader& shader, int frames_since_shot, int shot_cooldown);
    glm::vec3 getPos() const { return position; };
    void updatePos(glm::vec3 cameraFront);
    void setSpeed(float s) {speed = s; };
    void applyForce(glm::vec3 force_direction, float strength);

    void shoot(std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map);

    void requestOrbit(glm::vec3 planetPos, float orbitThreshold);



private:
    void buildVertices();
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);
    void drawLine(glm::vec3 start, glm::vec3 end);


    mutable std::vector<Sphere> trail;
    float maxSpeed = 10.1f;

    float speed = 5.5f;
    float size;
    glm::vec3 position;
    glm::mat4 modelMatrix;
    glm::vec3 direction;
    glm::vec3 force_direction;
    std::vector<glm::vec3> directions;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;

    mutable Sphere thruster;
    mutable Sphere aimer;

    std::vector<Bullet> bullets;

    int since_last_shot = 0;

    bool isOrbiting = false;
    glm::vec3 orbitPlanetPos;
    float orbitPlanetGravity = 0.0f;
    float orbitRange = 0.0f;



};

#endif
