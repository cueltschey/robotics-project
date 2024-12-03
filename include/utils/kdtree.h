#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

struct Point {
    float x, y, z;
};

struct KDNode {
    Point point;
    KDNode *left;
    KDNode *right;

    KDNode(Point p);
};

bool XLessThan(Point &left, Point &right);
bool YLessThan(Point &left, Point &right);
bool ZLessThan(Point &left, Point &right);

class KDTree {
public:
    KDTree(std::vector<Point> &points);

    void insert(Point newPoint);
    bool remove(Point oldPoint);
    void traverse(KDNode* node);

private:
    KDNode* root;

    KDNode* build(std::vector<Point> &points, int depth);
    void insert(KDNode* &node, Point newPoint, int depth);
    bool remove(KDNode* &node, Point oldPoint, int depth);
    KDNode* findMin(KDNode* node, int axis, int depth);
};

#endif
