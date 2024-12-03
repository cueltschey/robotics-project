#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

struct Point {
    float x, y, z;
};

struct KDNode {
    Point point;
    KDNode *left;
    KDNode *right;

    KDNode(Point p) : point(p), left(nullptr), right(nullptr) {};
};

bool XLessThan(Point& left, Point& right) {
    return left.x < right.x;
}

bool YLessThan(Point& left, Point& right) {
    return left.y < right.y;
}

bool ZLessThan(Point& left, Point& right) {
    return left.z < right.z;
}

class KDTree {
public:
    KDNode *root;

    KDTree(std::vector<Point>& points) {
        root = build(points, 0);
    }

    KDNode* build(std::vector<Point>& points, int depth) {
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

    void traverse(KDNode* node) {
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

    void insert(Point newPoint){
        std::cout << "Inserting point: (" << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << ")\n";
        insert(root, newPoint, 0);
    }

    void insert(KDNode* &node, Point newPoint, int depth) {
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
    
    bool remove(Point oldPoint) {
        return remove(root, oldPoint, 0);
    }

    bool remove(KDNode* &node, Point oldPoint, int depth) {
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

    KDNode* findMin(KDNode* node, int axis, int depth) {
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
};

// Global KDTree instance
KDTree* kdtree;

// A function to check and print OpenGL errors
void checkOpenGLError() {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
        switch (err) {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument." << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE: A numeric argument is out of range." << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION: The specified operation is not allowed in the current state." << std::endl;
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << "GL_STACK_OVERFLOW: This command would cause a stack overflow." << std::endl;
                break;
            case GL_STACK_UNDERFLOW:
                std::cerr << "GL_STACK_UNDERFLOW: This command would cause a stack underflow." << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command." << std::endl;
                break;
            default:
                std::cerr << "Unknown error." << std::endl;
                break;
        }
    }
}

// OpenGL render function
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear both color and depth buffers

    checkOpenGLError();

    // Render the KDTree points
    kdtree->traverse(kdtree->root);

    glfwSwapBuffers(glfwGetCurrentContext()); // Swap the window buffer
}

// OpenGL initialization for 3D
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Set background color
    glPointSize(10.0f);  // Set point size for visualization

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
}

// Update the camera position and orientation
void updateCamera(glm::mat4& view, glm::mat4& projection) {
    // Camera setup: LookAt with GLM (eye position, target position, up vector)
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f),  // Eye position
                       glm::vec3(0.0f, 0.0f, 0.0f),  // Target position (origin)
                       glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector

    // Perspective projection with GLM
    projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
}

int main(int argc, char** argv) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    // Create a windowed mode window and OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D KDTree Visualization", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW (make sure OpenGL context is created before calling glewInit)
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    } else {
        std::cout << "GLEW initialized successfully!" << std::endl;
    }

    // Set up OpenGL settings
    initOpenGL();

    // Build the KDTree
    std::vector<Point> points = {
        {0.1f, 0.2f, 0.3f}, {0.4f, 0.3f, 0.7f}, {0.7f, 0.8f, 0.9f},
        {0.3f, 0.6f, 0.4f}, {0.9f, 0.5f, 0.2f}, {0.2f, 0.7f, 0.1f},
        {-0.1f, -0.2f, 0.3f}, {-0.4f, -0.3f, 0.7f}, {-0.7f, -0.8f, 0.9f},
        {-0.3f, -0.6f, 0.4f}, {-0.9f, -0.5f, 0.2f}, {-0.2f, -0.7f, 0.1f}
    };
    kdtree = new KDTree(points);

    // GLM matrices for camera setup
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Main rendering loop
    float previousTime = 0.0f;
    float animationTime = 0.0f;
    Point newPoint = {0.0f, 0.0f, 0.0f};
    kdtree->insert(newPoint);

    while (!glfwWindowShouldClose(window)) {
        // Calculate deltaTime
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        if (animationTime < 1.0f) {
            animationTime += deltaTime;  // Update animation time
            Point oldPoint = newPoint;
            newPoint.x = glm::mix(0.0f, 1.0f, animationTime);  // Smoothly interpolate between 0.0f and 1.0f
            std::cout << "New X Position: " << newPoint.x << std::endl;

            bool works = kdtree->remove(oldPoint);  // Remove the old position
            std::cout << "Removal was " << works << std::endl;
            kdtree->insert(newPoint);   // Insert the new position
        } else {
            animationTime = 0.0f;  // Reset after animation completes
        }

        // Update camera
        updateCamera(view, projection);

        // Set the view and projection matrices
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(view));
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection));

        // Render the scene
        renderScene();

        // Poll for events
        glfwPollEvents();
    }

    // Clean up and exit
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}