#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <cmath>

class Boid {
public:
    Boid(float size, glm::vec3 start_pos);

    void updatePos();
    void applyForce(glm::vec3 force_direction, float strength);
    void draw() const;

    float getX() const { return position.x; }
    float getY() const { return position.y; }
    float getZ() const { return position.z; }
    glm::vec3 getPos() const { return position; };

private:
    void buildVertices();
    glm::vec3 rotateVertex(const glm::vec3& vertex, const glm::vec3& direction);

    float size;
    glm::vec3 position;
    glm::mat4 modelMatrix;

    glm::vec3 direction;


    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    GLuint VAO, VBO, EBO;
};

