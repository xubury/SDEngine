#ifndef SD_TRANSFORM_HPP
#define SD_TRANSFORM_HPP

#include "Common/Export.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <set>

namespace sd {

class SD_API Transform {
   public:
    Transform();
    virtual ~Transform() = default;

    Transform *getParent();
    const Transform *getParent() const;

    std::set<Transform *> &getChildren();
    const std::set<Transform *> &getChildren() const;

    void addChild(Transform *child);
    void removeChild(Transform *child);

    void apply(const Transform &transform);

    void translateLocal(const glm::vec3 &t);
    void translateWorld(const glm::vec3 &t);

    void rotateLocal(const glm::quat &rotation);
    void rotateWorld(const glm::quat &rotation);

    void setLocalPosition(const glm::vec3 &position);
    void setLocalRotation(const glm::quat &rotation);
    void setLocalScale(const glm::vec3 &scale);
    void setLocalTransform(const glm::mat4 &transform);

    glm::vec3 getLocalPosition() const;
    glm::quat getLocalRotation() const;
    glm::vec3 getLocalScale() const;
    glm::vec3 getLocalEulerAngle() const;
    glm::mat4 getLocalTransform() const;

    void setWorldPosition(const glm::vec3 &position);
    void setWorldRotation(const glm::quat &rotation);
    void setWorldScale(const glm::vec3 &scale);
    void setWorldTransform(const glm::mat4 &transform);

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
    void updateGlobalPosition();
    void updateGlobalRotation();
    void updateGlobalScale();

    void updateLocalPosition();
    void updateLocalRotation();
    void updateLocalScale();

    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    glm::vec3 m_localPosition;
    glm::quat m_localRotation;
    glm::vec3 m_localScale;

    std::set<Transform *> m_children;
    Transform *m_parent;
};

}  // namespace sd

#endif /* SD_TRANSFORM_HPP */
