#include "utils/generation.h"

#include <tuple>
#include <glm/glm.hpp>
#include <unordered_map>
#include <random>

#include <functional>

#include "utils/model.h"
#include "shapes/box.h"
#include "shapes/boid.h"

std::tuple<int, int, int> positionToCell(const glm::vec3& pos) {
    int cellX = static_cast<int>(std::ceil(pos.x) / CELL_SIZE);
    int cellY = static_cast<int>(std::ceil(pos.y) / CELL_SIZE);
    int cellZ = static_cast<int>(std::ceil(pos.z) / CELL_SIZE);

    return std::make_tuple(cellX, cellY, cellZ);
}

void drawChunkBorders(const std::unordered_map<std::tuple<int, int, int>, std::vector<Obstacle>>& box_map) {
    // Define the half size of a cell
    float halfCellSize = CELL_SIZE / 2.0f;

    for (const auto& [cell, boxes] : box_map) {
        int cellX, cellY, cellZ;
        std::tie(cellX, cellY, cellZ) = cell;

        // Calculate the center of this cell
        glm::vec3 center(cellX * CELL_SIZE, cellY * CELL_SIZE, cellZ * CELL_SIZE);

        // Define the 8 corners of the wireframe box
        glm::vec3 corners[8] = {
            center + glm::vec3(-halfCellSize, -halfCellSize, -halfCellSize),
            center + glm::vec3(halfCellSize, -halfCellSize, -halfCellSize),
            center + glm::vec3(halfCellSize, halfCellSize, -halfCellSize),
            center + glm::vec3(-halfCellSize, halfCellSize, -halfCellSize),
            center + glm::vec3(-halfCellSize, -halfCellSize, halfCellSize),
            center + glm::vec3(halfCellSize, -halfCellSize, halfCellSize),
            center + glm::vec3(halfCellSize, halfCellSize, halfCellSize),
            center + glm::vec3(-halfCellSize, halfCellSize, halfCellSize),
        };

        // Set color for the cell borders (e.g., light gray)
        glColor3f(0.7f, 0.7f, 0.7f);

        // Draw edges of the cube
        glBegin(GL_LINES);
        for (int i = 0; i < 4; ++i) {
            // Bottom square
            glVertex3fv(&corners[i].x);
            glVertex3fv(&corners[(i + 1) % 4].x);

            // Top square
            glVertex3fv(&corners[i + 4].x);
            glVertex3fv(&corners[(i + 1) % 4 + 4].x);

            // Vertical lines connecting top and bottom squares
            glVertex3fv(&corners[i].x);
            glVertex3fv(&corners[i + 4].x);
        }
        glEnd();
    }
}


glm::vec3 getRandomPointOutsideObstacles(
    std::unordered_map<std::tuple<int,int,int>, std::vector<Obstacle*>>& box_map,
    float maxPosition,
    float minDistance = 0.5f) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-maxPosition, maxPosition);

    glm::vec3 randomPoint;
    bool isValid;

    do {
        // Generate a random point
        randomPoint = glm::vec3(posDist(gen), posDist(gen), posDist(gen));
        isValid = true;

        std::vector<Obstacle*>& boxes = box_map[positionToCell(randomPoint)];

        // Check the point against each box to ensure it’s outside by at least minDistance
        for (const Obstacle* box : boxes) {
            glm::vec3 boxMin(box->getX() - box->getWidth() / 2 - minDistance,
                             box->getY() - box->getHeight() / 2 - minDistance,
                             box->getZ() - box->getDepth() / 2 - minDistance);

            glm::vec3 boxMax(box->getX() + box->getWidth() / 2 + minDistance,
                             box->getY() + box->getHeight() / 2 + minDistance,
                             box->getZ() + box->getDepth() / 2 + minDistance);

            // Check if the point is within the box bounds extended by minDistance
            if (randomPoint.x >= boxMin.x && randomPoint.x <= boxMax.x &&
                randomPoint.y >= boxMin.y && randomPoint.y <= boxMax.y &&
                randomPoint.z >= boxMin.z && randomPoint.z <= boxMax.z) {
                isValid = false;
                break;
            }
        }
    } while (!isValid); // Repeat until a valid point is found

    return randomPoint;
}


std::unordered_map<std::tuple<int,int,int>, std::vector<Obstacle*>> generateRandomBoxes(
    int numObstaclees, float maxSize, float maxPosition) {
    std::unordered_map<std::tuple<int,int,int>, std::vector<Obstacle*>> result;

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the random distributions
    std::uniform_real_distribution<float> sizeDist(0.5f, maxSize);        // Obstacle size range
    std::uniform_real_distribution<float> posDist(-maxPosition, maxPosition); // Position range
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    // Create random boxes
    for (int i = 0; i < numObstaclees; ++i) {
        float width = sizeDist(gen);
        float height = sizeDist(gen);
        float depth = sizeDist(gen);

        float x = posDist(gen);
        float y = posDist(gen);
        float z = posDist(gen);

        std::tuple<int,int,int> cellKey = positionToCell(glm::vec3(x,y,z));

        float r = colorDist(gen);
        float g = colorDist(gen);
        float b = colorDist(gen);

        result[cellKey].push_back(new Box(width, height, depth, x, y, z, r, g, b));
    }

    return result;
}

void generateRandomBoids(
    std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>>& result,
    int count,
    int maxDistance,
    std::unordered_map<std::tuple<int, int, int>, std::vector<Obstacle*>>& box_map,
    long int frame, glm::vec3 playerPos
    ){

    if(count <= 0)
      return;

    for (int i = 0; i < count; ++i) {
      glm::vec3 randomPos = playerPos + getRandomPointOutsideObstacles(box_map, maxDistance);
      result[positionToCell(randomPos)].push_back(
          Boid(frame, randomPos)
          );
    }
}

std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> recalculateCells(
    std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> old_map, int& num_boids
    ){

    std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> new_map;
    num_boids = 0;
    for(auto& [cell, boids] : old_map){
      for(Boid& boid : boids){
        new_map[positionToCell(boid.getPos())].push_back(boid);
        num_boids++;
      }
    }
    return new_map;
}

bool shouldSpawnBoid(long frame) {
    // Base probability of spawning, which increases with the frame count
    float spawnProbability = std::min(0.01f + (frame / 100000.0f), 1.0f); // Caps at 100% chance

    // Generate a random float between 0 and 1
    float randomValue = static_cast<float>(rand()) / RAND_MAX;

    // Spawn a boid if the random value is below the calculated probability
    return randomValue < spawnProbability;
}
