#ifndef SD_TRANSFORM_HPP
#define SD_TRANSFORM_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <set>

namespace SD {

bool SD_UTILITY_API Decompose(const glm::mat4& transform,
                              glm::vec3& translation, glm::quat& rotation,
                              glm::vec3& scale);

class SD_UTILITY_API Transform {
   public:
    Transform();
    virtual ~Transform() = default;

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    void SetTransform(const glm::mat4& transform);

    glm::vec3 GetPosition() const;
    glm::quat GetRotation() const;
    glm::vec3 GetScale() const;
    glm::mat4 GetMatrix() const;

    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;
    glm::vec3 GetFront() const;

    glm::vec3 WorldToLocal(const glm::vec3& world) const;
    glm::vec3 LocalToWorld(const glm::vec3& local) const;
    glm::vec3 WorldToLocalVector(const glm::vec3& world_vec) const;
    glm::vec3 LocalToWorldVector(const glm::vec3& local_vec) const;

    SERIALIZE(m_position, m_rotation, m_scale)

   private:
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;
};

}  // namespace SD

#endif /* SD_TRANSFORM_HPP */
