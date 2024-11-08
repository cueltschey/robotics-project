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

// Manhattan distance heuristic
float manhattanDistance(const glm::vec3& a, const glm::vec3& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

std::vector<glm::vec3> aStar(glm::vec3 start, glm::vec3 goal, const std::vector<Box>& boxes) {
    // Define the grid size and movement directions
    const float gridSize = 1.0f; // size of each grid cell
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


    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_set<glm::vec3> closedSet;
    Node* startNode = new Node(start);
    startNode->g = 0.0f;
    startNode->h = manhattanDistance(start, goal);
    startNode->f = startNode->g + startNode->h;
    openSet.push(*startNode);

    std::vector<Node*> allNodes;
    allNodes.push_back(startNode);

    Node* currentNode;

    while (!openSet.empty()) {
        currentNode = new Node(openSet.top());
        openSet.pop();

        // If the goal is reached, reconstruct the path
         if (glm::distance(currentNode->position, goal) <= 1.0f){
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

            glm::vec3 neighborPos = currentNode->position + dir * gridSize;

            // TODO: add box check
            Node* neighborNode = new Node(neighborPos);
            neighborNode->g = currentNode->g + gridSize;
            neighborNode->h = manhattanDistance(neighborPos, goal);
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

void drawPath(const std::vector<glm::vec3>& path) {
    glLineWidth(5.0f);

    glBegin(GL_LINES);
    for (size_t i = 1; i < path.size(); ++i) {
        glVertex3f(path[i - 1].x, path[i - 1].y, path[i - 1].z);
        glVertex3f(path[i].x, path[i].y, path[i].z);
    }
    glEnd();
}
#endif

