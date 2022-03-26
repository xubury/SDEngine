#ifndef SD_TRANSFORM_HPP
#define SD_TRANSFORM_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Math.hpp"

#include <set>

namespace SD {

class SD_UTILITY_API Transform {
   public:
    Transform();
    virtual ~Transform() = default;

    void SetPosition(const Vector3f& position);
    void SetRotation(const Quaternion& rotation);
    void SetScale(const Vector3f& scale);
    void SetTransform(const Matrix4f& transform);

    Vector3f GetPosition() const;
    Quaternion GetRotation() const;
    Vector3f GetScale() const;
    Matrix4f GetMatrix() const;

    Vector3f GetRight() const;
    Vector3f GetUp() const;
    Vector3f GetFront() const;

    Vector3f WorldToLocal(const Vector3f& world) const;
    Vector3f LocalToWorld(const Vector3f& local) const;
    Vector3f WorldToLocalVector(const Vector3f& world_vec) const;
    Vector3f LocalToWorldVector(const Vector3f& local_vec) const;

    SERIALIZE(m_position, m_rotation, m_scale)

   private:
    Vector3f m_position;
    Quaternion m_rotation;
    Vector3f m_scale;
};

}  // namespace SD

#endif /* SD_TRANSFORM_HPP */
