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
      Shader& shader);
    glm::vec3 getPos() const { return position; };
    void updatePos(glm::vec3 cameraFront);
    void setSpeed(float s) {speed = s; };
    void applyForce(glm::vec3 force_direction, float strength);

    void shoot(glm::vec3 cameraFront,
        std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>>& boid_map);


private:
    std::tuple<int, int, int> positionToCell(const glm::vec3& pos);
    void buildVertices();
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);
    void drawLine(glm::vec3 start, glm::vec3 end);


    mutable std::vector<Sphere> trail;

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

    mutable Sphere turret_sphere;
    mutable Cylinder turret_barrel;

    std::vector<Bullet> bullets;

    int since_last_shot = 0;


};

#endif
