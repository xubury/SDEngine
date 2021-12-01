#ifndef SD_TRANSFORM_HPP
#define SD_TRANSFORM_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <set>

namespace SD {

bool SD_UTILITY_API Decompose(const glm::mat4& transform, glm::vec3& translation,
                      glm::quat& rotation, glm::vec3& scale);

class SD_UTILITY_API Transform {
   public:
    Transform();
    virtual ~Transform() = default;

    Transform* GetParent();
    const Transform* GetParent() const;

    std::set<Transform*>& GetChildren();
    const std::set<Transform*>& GetChildren() const;

    void AddChild(Transform* child);
    void RemoveChild(Transform* child);

    void TranslateLocal(const glm::vec3& t);
    void TranslateWorld(const glm::vec3& t);

    void RotateLocal(const glm::quat& rotation);
    void RotateWorld(const glm::quat& rotation);

    void SetLocalPosition(const glm::vec3& position);
    void SetLocalRotation(const glm::quat& rotation);
    void SetLocalScale(const glm::vec3& scale);
    void SetLocalTransform(const glm::mat4& transform);

    glm::vec3 GetLocalPosition() const;
    glm::quat GetLocalRotation() const;
    glm::vec3 GetLocalScale() const;
    glm::mat4 GetLocalTransform() const;

    void SetWorldPosition(const glm::vec3& position);
    void SetWorldRotation(const glm::quat& rotation);
    void SetWorldScale(const glm::vec3& scale);
    void SetWorldTransform(const glm::mat4& transform);

    glm::vec3 GetWorldPosition() const;
    glm::quat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;
    glm::mat4 GetWorldTransform() const;

    glm::vec3 GetLocalRight() const;
    glm::vec3 GetLocalUp() const;
    glm::vec3 GetLocalFront() const;

    glm::vec3 GetWorldRight() const;
    glm::vec3 GetWorldUp() const;
    glm::vec3 GetWorldFront() const;

    glm::vec3 ToLocalSpace(const glm::vec3& world) const;
    glm::vec3 ToWorldSpace(const glm::vec3& local) const;
    glm::vec3 ToLocalVector(const glm::vec3& world_vec) const;
    glm::vec3 ToWorldVector(const glm::vec3& local_vec) const;

    SERIALIZE(m_position, m_rotation, m_scale, m_local_position,
              m_local_rotation, m_local_scale)

   private:
    void UpdateGlobalPosition();
    void UpdateGlobalRotation();
    void UpdateGlobalScale();

    void UpdateLocalPosition();
    void UpdateLocalRotation();
    void UpdateLocalScale();

    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    glm::vec3 m_local_position;
    glm::quat m_local_rotation;
    glm::vec3 m_local_scale;

    std::set<Transform*> m_children;
    Transform* m_parent;
};

}  // namespace SD

#endif /* SD_TRANSFORM_HPP */
