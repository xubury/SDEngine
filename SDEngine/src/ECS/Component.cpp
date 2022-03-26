#include "ECS/Component.hpp"

namespace SD {

TransformComponent::TransformComponent() : parent(entt::null) {}

void TransformComponent::SetLocalPosition(const Vector3f &position)
{
    m_local_transform.SetPosition(position);
    UpdateGlobalPosition();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetLocalRotation(const Quaternion &rotation)
{
    m_local_transform.SetRotation(rotation);
    UpdateGlobalRotation();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalRotation();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetLocalScale(const Vector3f &scale)
{
    m_local_transform.SetScale(scale);
    UpdateGlobalScale();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalScale();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetLocalTransform(const Matrix4f &trans)
{
    m_local_transform.SetTransform(trans);
    UpdateGlobalScale();
    UpdateGlobalRotation();
    UpdateGlobalPosition();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalScale();
        ecs->get<TransformComponent>(child).UpdateGlobalRotation();
        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

Vector3f TransformComponent::GetLocalPosition() const
{
    return m_local_transform.GetPosition();
}

Quaternion TransformComponent::GetLocalRotation() const
{
    return m_local_transform.GetRotation();
}

Vector3f TransformComponent::GetLocalScale() const
{
    return m_local_transform.GetScale();
}

const Transform &TransformComponent::GetLocalTransform() const
{
    return m_local_transform;
}

void TransformComponent::SetWorldPosition(const Vector3f &position)
{
    m_world_transform.SetPosition(position);
    UpdateLocalPosition();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetWorldRotation(const Quaternion &rotation)
{
    m_world_transform.SetRotation(rotation);
    UpdateLocalRotation();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalRotation();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetWorldScale(const Vector3f &scale)
{
    m_world_transform.SetScale(scale);
    UpdateLocalScale();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalScale();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetWorldTransform(const Matrix4f &trans)
{
    m_world_transform.SetTransform(trans);
    UpdateLocalPosition();
    UpdateLocalRotation();
    UpdateLocalScale();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
        ecs->get<TransformComponent>(child).UpdateGlobalRotation();
        ecs->get<TransformComponent>(child).UpdateGlobalScale();
    }
}

Vector3f TransformComponent::GetWorldPosition() const
{
    return m_world_transform.GetPosition();
}

Quaternion TransformComponent::GetWorldRotation() const
{
    return m_world_transform.GetRotation();
}

Vector3f TransformComponent::GetWorldScale() const
{
    return m_world_transform.GetScale();
}

const Transform &TransformComponent::GetWorldTransform() const
{
    return m_world_transform;
}

Vector3f TransformComponent::GetLocalRight() const
{
    return m_local_transform.GetRotation() * Vector3f(1.f, 0.f, 0.f);
}

Vector3f TransformComponent::GetLocalUp() const
{
    return m_local_transform.GetRotation() * Vector3f(0.f, 1.f, 0.f);
}

Vector3f TransformComponent::GetLocalFront() const
{
    return m_local_transform.GetRotation() * Vector3f(0.f, 0.f, 1.f);
}

Vector3f TransformComponent::GetWorldRight() const
{
    return m_world_transform.GetRotation() * Vector3f(1.f, 0.f, 0.f);
}

Vector3f TransformComponent::GetWorldUp() const
{
    return m_world_transform.GetRotation() * Vector3f(0.f, 1.f, 0.f);
}

Vector3f TransformComponent::GetWorldFront() const
{
    return m_world_transform.GetRotation() * Vector3f(0.f, 0.f, 1.f);
}

void TransformComponent::UpdateGlobalPosition()
{
    if (parent == entt::null)
        m_world_transform.SetPosition(m_local_transform.GetPosition());
    else {
        Matrix4f global =
            ecs->get<TransformComponent>(parent).m_world_transform.GetMatrix() *
            m_local_transform.GetMatrix();
        m_world_transform.SetPosition(global[3]);
    }
    for (auto child : children) {
        auto &t = ecs->get<TransformComponent>(child);
        t.UpdateGlobalPosition();
    }
}

void TransformComponent::UpdateGlobalRotation()
{
    if (parent == entt::null)
        m_world_transform.SetRotation(m_local_transform.GetRotation());
    else {
        Matrix4f global = glm::toMat4(ecs->get<TransformComponent>(parent)
                                          .m_world_transform.GetRotation() *
                                      m_local_transform.GetRotation());
        m_world_transform.SetRotation(Quaternion(global));
    }
    for (auto child : children) {
        auto &t = ecs->get<TransformComponent>(child);
        t.UpdateGlobalRotation();
    }
}

void TransformComponent::UpdateGlobalScale()
{
    if (parent == entt::null)
        m_world_transform.SetScale(m_local_transform.GetScale());
    else {
        Matrix4f global =
            glm::scale(Matrix4f(1.0f), ecs->get<TransformComponent>(parent)
                                           .m_world_transform.GetScale()) *
            glm::scale(Matrix4f(1.0f), m_local_transform.GetScale());
        m_world_transform.SetScale(
            Vector3f(global[0][0], global[1][1], global[2][2]));
    }
    for (auto child : children) {
        auto &t = ecs->get<TransformComponent>(child);
        t.UpdateGlobalScale();
    }
}

void TransformComponent::UpdateLocalPosition()
{
    if (parent == entt::null)
        m_local_transform.SetPosition(m_world_transform.GetPosition());
    else {
        Matrix4f global = glm::inverse(ecs->get<TransformComponent>(parent)
                                           .m_world_transform.GetMatrix()) *
                          m_world_transform.GetMatrix();
        m_local_transform.SetPosition(global[3]);
    }
}

void TransformComponent::UpdateLocalRotation()
{
    if (parent == entt::null)
        m_local_transform.SetRotation(m_world_transform.GetRotation());
    else {
        Matrix4f global =
            glm::inverse(glm::toMat4(ecs->get<TransformComponent>(parent)
                                         .m_world_transform.GetRotation())) *
            glm::toMat4(m_world_transform.GetRotation());
        m_local_transform.SetRotation(Quaternion(global));
    }
}

void TransformComponent::UpdateLocalScale()
{
    if (parent == entt::null)
        m_local_transform.SetScale(m_local_transform.GetScale());
    else {
        Matrix4f global =
            glm::inverse(
                glm::scale(Matrix4f(1.0f), ecs->get<TransformComponent>(parent)
                                               .m_world_transform.GetScale())) *
            glm::scale(Matrix4f(1.0f), m_world_transform.GetScale());
        m_local_transform.SetScale(
            Vector3f(global[0][0], global[1][1], global[2][2]));
    }
}
}  // namespace SD
