#ifndef ASTAR_H
#define ASTAR_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shapes/sphere.h"
#include "shapes/box.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/camera.h"
#include <optional>
#include "utils/m_shader.h"
#include "utils/scene.h"
#include <ctime>
#include <random>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_set>
//Caden Adding Heuristics
std::vector<glm::vec3> ray_points;




namespace std {
    template <>
    struct hash<glm::vec3> {
        size_t operator()(const glm::vec3& v) const {
            size_t h1 = std::hash<float>()(v.x);
            size_t h2 = std::hash<float>()(v.y);
            size_t h3 = std::hash<float>()(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

struct Node {
    glm::vec3 position;
    float g = FLT_MAX;  // Cost from start
    float h = 0.0f;     // Heuristic cost to goal
    float f = FLT_MAX;  // Total cost
    Node* parent = nullptr;

    Node(glm::vec3 pos) : position(pos) {}

    // Comparison operator for priority queue (min-heap)
    bool operator>(const Node& other) const {
        return f > other.f;
    }
};




//Euclidean Distance heuristic
float euclideanDistance(const glm::vec3& a, const glm::vec3& b)
{
    return std::sqrt((std::pow(a.x -b.x,2)) + (std::pow(a.y-b.y,2)) + (std::pow(a.z-b.z,2)));
}

//Chebyshev Distance heuristic
float chebyshevDistance(const glm::vec3& a, const glm::vec3& b)
{
    float cadenvar1 = std::max(std::abs(a.x -b.x),std::abs(a.y - b.y));
    return std::max(cadenvar1,std::abs(a.z - b.z));
}

// Manhattan distance heuristic
float manhattanDistance(const glm::vec3& a, const glm::vec3& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

std::vector<glm::vec3> aStar(glm::vec3 start, glm::vec3 goal, const std::vector<Box>& boxes) {
    // Define the grid size and movement directions
    const float gridSize = 0.01f; // size of each grid cell
    

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_set<glm::vec3> closedSet;
    Node* startNode = new Node(start);
    startNode->g = 0.0f;
    startNode->h = manhattanDistance(start, goal);
    //neighborNode->h = chebyshevDistance(start, goal);
    //neighborNode->h = euclideanDistance(start, goal);
    startNode->f = startNode->g + startNode->h;
    openSet.push(*startNode);

    std::vector<Node*> allNodes;
    allNodes.push_back(startNode);

    Node* currentNode;

    bool first = true;

    ray_points.clear();

    while (!openSet.empty()) {
        currentNode = new Node(openSet.top());
        openSet.pop();


        // If the goal is reached, reconstruct the path
         if (glm::distance(currentNode->position, goal) <= 0.2f){
            std::vector<glm::vec3> path;
            while (currentNode != nullptr) {
                path.push_back(currentNode->position);
                currentNode = currentNode->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;  // Return the path
        }

        closedSet.insert(currentNode->position);





        // Explore neighbors
        for (const glm::vec3& dir : directions) {
            bool invalid = false;

            glm::vec3 neighborPos = currentNode->position + dir * gridSize;


            Node* neighborNode = new Node(neighborPos);
            neighborNode->g = currentNode->g + gridSize;
            neighborNode->h = manhattanDistance(neighborPos, goal);
            //neighborNode->h = chebyshevDistance(neighborPos, goal);
            //neighborNode->h = euclideanDistance(neighborPos, goal);
            neighborNode->f = neighborNode->g + neighborNode->h;
            neighborNode->parent = currentNode;

            openSet.push(*neighborNode);
            allNodes.push_back(neighborNode);
        }
    }


    std::vector<glm::vec3> path;
    while (currentNode != nullptr) {
        path.push_back(currentNode->position);
        currentNode = currentNode->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;  // Return the path
}

void drawPath(const std::vector<glm::vec3>& path, glm::vec3 boid_pos) {
    glLineWidth(5.0f);

    glBegin(GL_LINES);
    for (size_t i = 1; i < path.size(); ++i) {
        glVertex3f(path[i - 1].x, path[i - 1].y, path[i - 1].z);
        glVertex3f(path[i].x, path[i].y, path[i].z);
    }

    for (const auto& end : ray_points) {
        glVertex3f(boid_pos.x, boid_pos.y, boid_pos.z);
        glVertex3f(end.x, end.y, end.z);
    }

    glEnd();
}
#endif

