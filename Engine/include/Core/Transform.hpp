#ifndef SD_TRANSFORM_HPP
#define SD_TRANSFORM_HPP

#include "Common/Export.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

namespace sd {

class SD_API Transform {
   public:
    Transform();
    virtual ~Transform() = default;

    Transform *parent();
    const Transform *parent() const;

    std::vector<Transform *> &children();
    const std::vector<Transform *> &children() const;

    void setParent(Transform *transform, bool cleanTree = true);
    void addChild(Transform *transform);
    void removeChild(Transform *transform);

    void apply(const Transform &transform);

    void translateLocal(const glm::vec3 &t);
    void translateWorld(const glm::vec3 &t);

    void rotateLocal(float radians, const glm::vec3 &axis);
    void rotateWorld(float radians, const glm::vec3 &axis);

    void rotateLocal(const glm::quat &rotation);
    void rotateWorld(const glm::quat &rotation);

    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::quat &rotation);
    void setEulerAngle(glm::vec3 eulerAngle);
    void setTransform(const glm::mat4 &transform);

    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    glm::vec3 getScale() const;
    glm::vec3 getEulerAngle() const;
    glm::mat4 getTransform() const;

    glm::vec3 getWorldPosition() const;
    glm::quat getWorldRotation() const;
    glm::vec3 getWorldScale() const;
    glm::vec3 getWorldEulerAngle() const;
    glm::mat4 getWorldTransform() const;

    glm::vec3 getRight() const;
    glm::vec3 getUp() const;
    glm::vec3 getFront() const;

    glm::vec3 getWorldRight() const;
    glm::vec3 getWorldUp() const;
    glm::vec3 getWorldFront() const;

    glm::vec3 toLocalSpace(const glm::vec3 &world) const;
    glm::vec3 toWorldSpace(const glm::vec3 &local) const;
    glm::vec3 toLocalVector(const glm::vec3 &worldVec) const;
    glm::vec3 toWorldVector(const glm::vec3 &localVec) const;

   private:
    glm::vec3 getParentPosition() const;
    glm::quat getParentRotation() const;
    glm::vec3 getParentScale() const;

    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    std::vector<Transform *> m_children;
    Transform *m_parent;
};

}  // namespace sd

#endif /* SD_TRANSFORM_HPP */
