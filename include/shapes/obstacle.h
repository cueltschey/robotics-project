
class Obstacle {
public:
    virtual ~Obstacle() = default;

    // Mark all getters as virtual with the 'const' qualifier to allow overriding
    virtual float getMinX() const { return 0.0f; }
    virtual float getMaxX() const { return 0.0f; }
    virtual float getMinY() const { return 0.0f; }
    virtual float getMaxY() const { return 0.0f; }
    virtual float getMinZ() const { return 0.0f; }
    virtual float getMaxZ() const { return 0.0f; }

    virtual float getX() const { return 0.0f; }
    virtual float getY() const { return 0.0f; }
    virtual float getZ() const { return 0.0f; }

    // Mark width, height, depth, and contains as virtual
    virtual float getWidth() const { return 0.0f; }
    virtual float getHeight() const { return 0.0f; }
    virtual float getDepth() const { return 0.0f; }

    virtual bool contains(const glm::vec3& point) const { return false; }
};

