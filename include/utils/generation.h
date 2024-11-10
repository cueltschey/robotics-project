#ifndef GENERATION_H
#define GENERATION_H

#include <tuple>
#include <glm/glm.hpp>
#include <unordered_map>

#include <functional>

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

#define CELL_SIZE 3.0f

std::tuple<int, int, int> positionToCell(const glm::vec3& pos) {
    int cellX = static_cast<int>(pos.x / CELL_SIZE);
    int cellY = static_cast<int>(pos.y / CELL_SIZE);
    int cellZ = static_cast<int>(pos.z / CELL_SIZE);

    return std::make_tuple(cellX, cellY, cellZ);
}

glm::vec3 getRandomPointOutsideBoxes(
    std::unordered_map<std::tuple<int,int,int>, std::vector<Box>>& box_map,
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

        std::vector<Box>& boxes = box_map[positionToCell(randomPoint)];

        // Check the point against each box to ensure it’s outside by at least minDistance
        for (const Box& box : boxes) {
            glm::vec3 boxMin(box.getX() - box.getWidth() / 2 - minDistance,
                             box.getY() - box.getHeight() / 2 - minDistance,
                             box.getZ() - box.getDepth() / 2 - minDistance);

            glm::vec3 boxMax(box.getX() + box.getWidth() / 2 + minDistance,
                             box.getY() + box.getHeight() / 2 + minDistance,
                             box.getZ() + box.getDepth() / 2 + minDistance);

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



std::unordered_map<std::tuple<int,int,int>, std::vector<Box>> generateRandomBoxes(
    int numBoxes, float maxSize, float maxPosition) {
    std::unordered_map<std::tuple<int,int,int>, std::vector<Box>> result;

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the random distributions
    std::uniform_real_distribution<float> sizeDist(0.5f, maxSize);        // Box size range
    std::uniform_real_distribution<float> posDist(-maxPosition, maxPosition); // Position range
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    // Create random boxes
    for (int i = 0; i < numBoxes; ++i) {
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

        result[cellKey].push_back(Box(width, height, depth, x, y, z, r, g, b));
    }

    return result;
}

void generateRandomBoids(
    std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>>& result,
    int count,
    int maxDistance,
    std::unordered_map<std::tuple<int, int, int>, std::vector<Box>>& box_map,
    BoidParams& params){

    for (int i = 0; i < count; ++i) {
      glm::vec3 randomPosition = getRandomPointOutsideBoxes(box_map, maxDistance);
      result[positionToCell(randomPosition)].push_back(Boid(randomPosition, params));
    }
}

std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> recalculateCells(
    std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> old_map
    ){

    std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> new_map;
    for(auto& [cell, boids] : old_map){
      for(Boid& boid : boids){
        new_map[positionToCell(boid.getPos())].push_back(boid);
      }
    }
    return new_map;
}


#endif // !GENERATION_H
