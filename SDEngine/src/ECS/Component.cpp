#include "ECS/Component.hpp"

namespace SD {

TransformComponent::TransformComponent() : parent(entt::null) {}

void TransformComponent::SetLocalPosition(const glm::vec3 &position) {
    m_local_transform.SetPosition(position);
    UpdateGlobalPosition();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetLocalRotation(const glm::quat &rotation) {
    m_local_transform.SetRotation(rotation);
    UpdateGlobalRotation();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalRotation();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetLocalScale(const glm::vec3 &scale) {
    m_local_transform.SetScale(scale);
    UpdateGlobalScale();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalScale();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetLocalTransform(const glm::mat4 &trans) {
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

glm::vec3 TransformComponent::GetLocalPosition() const {
    return m_local_transform.GetPosition();
}

glm::quat TransformComponent::GetLocalRotation() const {
    return m_local_transform.GetRotation();
}

glm::vec3 TransformComponent::GetLocalScale() const {
    return m_local_transform.GetScale();
}

const Transform &TransformComponent::GetLocalTransform() const {
    return m_local_transform;
}

void TransformComponent::SetWorldPosition(const glm::vec3 &position) {
    m_world_transform.SetPosition(position);
    UpdateLocalPosition();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetWorldRotation(const glm::quat &rotation) {
    m_world_transform.SetRotation(rotation);
    UpdateLocalRotation();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalRotation();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetWorldScale(const glm::vec3 &scale) {
    m_world_transform.SetScale(scale);
    UpdateLocalScale();
    for (auto child : children) {
        ecs->get<TransformComponent>(child).UpdateGlobalScale();

        ecs->get<TransformComponent>(child).UpdateGlobalPosition();
    }
}

void TransformComponent::SetWorldTransform(const glm::mat4 &trans) {
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

glm::vec3 TransformComponent::GetWorldPosition() const {
    return m_world_transform.GetPosition();
}

glm::quat TransformComponent::GetWorldRotation() const {
    return m_world_transform.GetRotation();
}

glm::vec3 TransformComponent::GetWorldScale() const {
    return m_world_transform.GetScale();
}

const Transform &TransformComponent::GetWorldTransform() const {
    return m_world_transform;
}

glm::vec3 TransformComponent::GetLocalRight() const {
    return m_local_transform.GetRotation() * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 TransformComponent::GetLocalUp() const {
    return m_local_transform.GetRotation() * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 TransformComponent::GetLocalFront() const {
    return m_local_transform.GetRotation() * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 TransformComponent::GetWorldRight() const {
    return m_world_transform.GetRotation() * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 TransformComponent::GetWorldUp() const {
    return m_world_transform.GetRotation() * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 TransformComponent::GetWorldFront() const {
    return m_world_transform.GetRotation() * glm::vec3(0.f, 0.f, 1.f);
}

void TransformComponent::UpdateGlobalPosition() {
    if (parent == entt::null)
        m_world_transform.SetPosition(m_local_transform.GetPosition());
    else {
        glm::mat4 global =
            ecs->get<TransformComponent>(parent).m_world_transform.GetMatrix() *
            m_local_transform.GetMatrix();
        m_world_transform.SetPosition(global[3]);
    }
    for (auto child : children) {
        auto &t = ecs->get<TransformComponent>(child);
        t.UpdateGlobalPosition();
    }
}

void TransformComponent::UpdateGlobalRotation() {
    if (parent == entt::null)
        m_world_transform.SetRotation(m_local_transform.GetRotation());
    else {
        glm::mat4 global = glm::toMat4(ecs->get<TransformComponent>(parent)
                                           .m_world_transform.GetRotation() *
                                       m_local_transform.GetRotation());
        m_world_transform.SetRotation(glm::quat(global));
    }
    for (auto child : children) {
        auto &t = ecs->get<TransformComponent>(child);
        t.UpdateGlobalRotation();
    }
}

void TransformComponent::UpdateGlobalScale() {
    if (parent == entt::null)
        m_world_transform.SetScale(m_local_transform.GetScale());
    else {
        glm::mat4 global =
            glm::scale(glm::mat4(1.0f), ecs->get<TransformComponent>(parent)
                                            .m_world_transform.GetScale()) *
            glm::scale(glm::mat4(1.0f), m_local_transform.GetScale());
        m_world_transform.SetScale(
            glm::vec3(global[0][0], global[1][1], global[2][2]));
    }
    for (auto child : children) {
        auto &t = ecs->get<TransformComponent>(child);
        t.UpdateGlobalScale();
    }
}

void TransformComponent::UpdateLocalPosition() {
    if (parent == entt::null)
        m_local_transform.SetPosition(m_world_transform.GetPosition());
    else {
        glm::mat4 global = glm::inverse(ecs->get<TransformComponent>(parent)
                                            .m_world_transform.GetMatrix()) *
                           m_world_transform.GetMatrix();
        m_local_transform.SetPosition(global[3]);
    }
}

void TransformComponent::UpdateLocalRotation() {
    if (parent == entt::null)
        m_local_transform.SetRotation(m_world_transform.GetRotation());
    else {
        glm::mat4 global =
            glm::inverse(glm::toMat4(ecs->get<TransformComponent>(parent)
                                         .m_world_transform.GetRotation())) *
            glm::toMat4(m_world_transform.GetRotation());
        m_local_transform.SetRotation(glm::quat(global));
    }
}

void TransformComponent::UpdateLocalScale() {
    if (parent == entt::null)
        m_local_transform.SetScale(m_local_transform.GetScale());
    else {
        glm::mat4 global =
            glm::inverse(glm::scale(glm::mat4(1.0f),
                                    ecs->get<TransformComponent>(parent)
                                        .m_world_transform.GetScale())) *
            glm::scale(glm::mat4(1.0f), m_world_transform.GetScale());
        m_local_transform.SetScale(
            glm::vec3(global[0][0], global[1][1], global[2][2]));
    }
}
}  // namespace SD
