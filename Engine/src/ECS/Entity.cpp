#include "ECS/Entity.hpp"

namespace SD {

const entt::entity Entity::INVALID_ID = entt::entity(-1);

Entity::Entity() : m_handle(entt::null), m_scene(nullptr) {}

Entity::Entity(entt::entity handle, Scene *scene)
    : m_handle(handle), m_scene(scene) {}

void Entity::Destroy(bool is_root) {
    auto &data = GetComponent<EntityDataComponent>();
    for (entt::entity entity_id : data.children) {
        Entity child(entity_id, m_scene);
        child.Destroy(false);
    }
    Entity parent(data.parent, m_scene);
    if (is_root && parent) {
        parent.RemoveChild(*this);
    }
    m_scene->destroy(*this);
}

Entity Entity::CreateChild(const std::string &name) {
    Entity child = m_scene->CreateEntity(name);
    AddChild(child);
    return child;
}

void Entity::AddChild(Entity &child) {
    if (*this == child) return;
    auto &parent_data = GetComponent<EntityDataComponent>();
    auto &child_data = child.GetComponent<EntityDataComponent>();
    if (parent_data.parent == child || child_data.parent == *this) return;

    Transform *child_transform =
        &child.GetComponent<TransformComponent>().transform;
    Transform *parent_transform = &GetComponent<TransformComponent>().transform;

    Entity old_parent(child_data.parent, child.m_scene);
    if (old_parent) {
        old_parent.RemoveChild(child);
    }
    parent_transform->AddChild(child_transform);
    parent_data.children.emplace(child);
    child_data.parent = *this;
}

void Entity::RemoveChild(Entity &child) {
    auto &children = GetComponent<EntityDataComponent>().children;
    auto iter = children.find(child);
    if (iter != children.end()) {
        children.erase(iter);
        Transform *childTransform =
            &child.GetComponent<TransformComponent>().transform;
        GetComponent<TransformComponent>().transform.RemoveChild(
            childTransform);
    }
    child.GetComponent<EntityDataComponent>().parent = Entity();
}

Entity::operator bool() const {
    return m_handle != entt::null && m_scene != nullptr;
}

Entity::operator entt::entity() const { return m_handle; }

bool Entity::operator!=(const Entity &other) const { return !(*this == other); }

bool Entity::operator==(const Entity &other) const {
    return m_handle == other.m_handle && m_scene == other.m_scene;
}

bool Entity::operator<(const Entity &other) const {
    return m_handle < other.m_handle;
}

}  // namespace SD
