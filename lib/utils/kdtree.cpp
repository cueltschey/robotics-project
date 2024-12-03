#include "utils/kdtree.h"

#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <algorithm>

KDNode::KDNode(Point p) : point(p), left(nullptr), right(nullptr) {};

bool XLessThan(Point &left, Point &right) {
    return left.x < right.x;
}

bool YLessThan(Point &left, Point &right) {
    return left.y < right.y;
}

bool ZLessThan(Point &left, Point &right) {
    return left.z < right.z;
}

KDTree::KDTree(std::vector<Point> &points) {
    root = build(points, 0);
}

KDNode* KDTree::build(std::vector<Point> &points, int depth) {
    if (points.empty()) {
        return nullptr;
    }

    int axis = depth % 3;
    if (axis == 0) {
        std::sort(points.begin(), points.end(), XLessThan);
    }
    else if (axis == 1) {
        std::sort(points.begin(), points.end(), YLessThan);
    }
    else {
        std::sort(points.begin(), points.end(), ZLessThan);
    }

    int medianIdx = points.size() / 2;
    KDNode* node = new KDNode(points[medianIdx]);

    std::vector<Point> leftPoints(points.begin(), points.begin() + medianIdx);
    std::vector<Point> rightPoints(points.begin() + medianIdx + 1, points.end());

    node->left = build(leftPoints, depth + 1);
    node->right = build(rightPoints, depth + 1);

    return node;
}

void KDTree::traverse(KDNode* node) {
    if (node == nullptr) {
        return;
    }

    traverse(node->left);
    traverse(node->right);
    std::cout << "Point in tree: (" << node->point.x << ", " << node->point.y << ", " << node->point.z << ")\n";

    glBegin(GL_POINTS);
    glVertex3f(node->point.x, node->point.y, node->point.z);
    glEnd();
}

void KDTree::insert(Point newPoint){
    std::cout << "Inserting point: (" << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << ")\n";
    insert(root, newPoint, 0);
}

void KDTree::insert(KDNode* &node, Point newPoint, int depth) {
    if (node == nullptr) {
        node = new KDNode(newPoint);
        std::cout << "Inserting at leaf: (" << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << ")\n";
        return;
    }

    int axis = depth % 3;
    std::cout << "At node: (" << node->point.x << ", " << node->point.y << ", " << node->point.z << ") axis: " << axis << std::endl;

    if (axis == 0) {
        if (newPoint.x < node->point.x) {
            std::cout << "Going left\n";
            insert(node->left, newPoint, depth + 1);
        }
        else {
            std::cout << "Going right\n";
            insert(node->right, newPoint, depth + 1);
        }
    }
    else if (axis == 1) {
        if (newPoint.y < node->point.y) {
            std::cout << "Going left\n";
            insert(node->left, newPoint, depth + 1);
        }
        else {
            std::cout << "Going right\n";
            insert(node->right, newPoint, depth + 1);
        }
    }
    else {
        if (newPoint.z < node->point.z) {
            std::cout << "Going left\n";
            insert(node->left, newPoint, depth + 1);
        }
        else {
            std::cout << "Going right\n";
            insert(node->right, newPoint, depth + 1);
        }
    }
}
    
bool KDTree::remove(Point oldPoint) {
    return remove(root, oldPoint, 0);
}

bool KDTree::remove(KDNode* &node, Point oldPoint, int depth) {
    std::cout << "Removing point: (" << oldPoint.x << ", " << oldPoint.y << ", " << oldPoint.z << ")\n"; 
    if (node == nullptr) {
        return false;
    }

    int axis = depth % 3;
    if ((axis == 0 && oldPoint.x < node->point.x) ||
        (axis == 1 && oldPoint.y < node->point.y) ||
        (axis == 2 && oldPoint.z < node->point.z)) {
            return remove(node->left, oldPoint, depth + 1);
    }
    else if ((axis == 0 && oldPoint.x > node->point.x) ||
                (axis == 1 && oldPoint.y > node->point.y) ||
                (axis == 2 && oldPoint.z > node->point.z)) {
            return remove(node->right, oldPoint, depth + 1);
    }
    else {
        std::cout << "Found point to remove: (" << node->point.x << ", " << node->point.y << ", " << node->point.z << ")\n";
        if (node->left != nullptr && node->right != nullptr) {
            KDNode* minNode = findMin(node->right, axis, depth + 1);
            node->point = minNode->point;
            return remove(node->right, minNode->point, depth + 1);
        }
        else {
            KDNode* temp = (node->left != nullptr) ? node->left : node->right;
            if (temp == nullptr) {
                temp = node;
                node = nullptr;
            }
            else {
                *node = *temp;
            }
            delete temp;
            return true;
        }
    }
}

KDNode* KDTree::findMin(KDNode* node, int axis, int depth) {
    if (node == nullptr) {
        return nullptr;
    }

    int nextAxis = (depth + 1) % 3;

    if (axis == 0) {
        if (node->left == nullptr) {
            return node;
        }
        return findMin(node->left, nextAxis, depth + 1);
    }
    else if (axis == 1) {
        if (node->right == nullptr) {
            return node;
        }
        return findMin(node->right, nextAxis, depth + 1);
    }
    return node;
}
