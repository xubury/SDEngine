#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace SD {

const entt::entity Entity::INVALID_ID = entt::null;

Entity::Entity() : m_handle(entt::null), m_scene(nullptr) {}

Entity::Entity(entt::entity handle, Scene *scene)
    : m_handle(handle), m_scene(scene) {}

void Entity::Destroy(bool is_root) {
    auto &data = GetComponent<TransformComponent>();
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
    auto &parent_data = GetComponent<TransformComponent>();
    auto &child_data = child.GetComponent<TransformComponent>();
    if (parent_data.parent == child || child_data.parent == *this) return;

    Entity old_parent(child_data.parent, child.m_scene);
    if (old_parent) {
        old_parent.RemoveChild(child);
    }
    parent_data.children.emplace(child);
    child_data.parent = *this;
}

void Entity::RemoveChild(Entity &child) {
    auto &children = GetComponent<TransformComponent>().children;
    auto iter = children.find(child);
    if (iter != children.end()) {
        children.erase(iter);
    }
    child.GetComponent<TransformComponent>().parent = Entity();
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
