#include "Utility/Transform.hpp"
#include "Utility/Log.hpp"

namespace SD {

Transform::Transform()
    : m_position(0.f), m_rotation(1.f, 0.f, 0.f, 0.f), m_scale(1.0f)
{
}

void Transform::SetPosition(const Vector3f &position) { m_position = position; }

void Transform::SetRotation(const Quaternion &rotation)
{
    m_rotation = rotation;
}

void Transform::SetScale(const Vector3f &scale) { m_scale = scale; }

void Transform::SetTransform(const Matrix4f &transform)
{
    Math::Decompose(transform, m_position, m_rotation, m_scale);
}

Vector3f Transform::GetPosition() const { return m_position; }

Quaternion Transform::GetRotation() const { return m_rotation; }

Vector3f Transform::GetScale() const { return m_scale; }

Matrix4f Transform::GetMatrix() const
{
    return glm::translate(Matrix4f(1.0f), m_position) * Matrix4f(m_rotation) *
           glm::scale(Matrix4f(1.0f), m_scale);
}

Vector3f Transform::GetRight() const
{
    return m_rotation * Vector3f(1.f, 0.f, 0.f);
}

Vector3f Transform::GetUp() const
{
    return m_rotation * Vector3f(0.f, 1.f, 0.f);
}

Vector3f Transform::GetFront() const
{
    return m_rotation * Vector3f(0.f, 0.f, 1.f);
}

Vector3f Transform::WorldToLocal(const Vector3f &world) const
{
    Matrix3f rotation(GetRotation());
    return glm::transpose(rotation) * (world - GetPosition());
}

Vector3f Transform::LocalToWorld(const Vector3f &local) const
{
    return GetRotation() * local + GetPosition();
}

Vector3f Transform::WorldToLocalVector(const Vector3f &world_vec) const
{
    return glm::transpose(glm::toMat3(GetRotation())) * world_vec;
}

Vector3f Transform::LocalToWorldVector(const Vector3f &local_vec) const
{
    return GetRotation() * local_vec;
}

}  // namespace SD
