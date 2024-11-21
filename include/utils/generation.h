#ifndef GENERATION_H
#define GENERATION_H

#include <tuple>
#include <glm/glm.hpp>
#include <unordered_map>
#include <random>

#include <functional>

#include "shapes/box.h"
#include "shapes/boid.h"

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

#define CELL_SIZE 5.0f

std::tuple<int, int, int> positionToCell(const glm::vec3& pos);

void drawChunkBorders(const std::unordered_map<std::tuple<int, int, int>, std::vector<Obstacle>>& box_map);



glm::vec3 getRandomPointOutsideObstacles(
    std::unordered_map<std::tuple<int,int,int>, std::vector<Obstacle*>>& box_map,
    float maxPosition,
    float minDistance);

std::unordered_map<std::tuple<int,int,int>, std::vector<Obstacle*>> generateRandomBoxes(
    int numObstaclees, float maxSize, float maxPosition);

void generateRandomBoids(
    std::unordered_map<std::tuple<int, int, int>, std::vector<Boid>>& result,
    int count,
    int maxDistance,
    std::unordered_map<std::tuple<int, int, int>, std::vector<Obstacle*>>& box_map,
    long int frame, glm::vec3 playerPos);

std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> recalculateCells(
    std::unordered_map<std::tuple<int,int,int>, std::vector<Boid>> old_map, int& num_boids);

bool shouldSpawnBoid(long frame);


#endif // !GENERATION_H
