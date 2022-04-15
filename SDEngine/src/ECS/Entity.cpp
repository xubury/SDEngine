#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace SD {

Entity::Entity() : m_handle(entt::null), m_scene(nullptr) {}

Entity::Entity(entt::entity handle, Scene *scene)
    : m_handle(handle), m_scene(scene)
{
}

void Entity::Destroy(bool is_root)
{
    auto &data = GetComponent<TransformComponent>();
    if (is_root) {
        Entity parent(data.parent, m_scene);
        if (parent) {
            parent.RemoveChild(*this);
        }
    }
    // Apprently destroy entity mess with component data
    // copy the data for destroy instead.
    auto children = data.children;
    for (entt::entity entity_id : children) {
        Entity child(entity_id, m_scene);
        child.Destroy(false);
    }
    m_scene->m_entity_reg.destroy(*this);
}

void Entity::AddChild(Entity &child)
{
    if (*this == child) {
        SD_CORE_WARN("Cannot add self as child!");
        return;
    };
    auto &data = GetComponent<TransformComponent>();
    auto &child_data = child.GetComponent<TransformComponent>();
    if (data.parent == child) {
        SD_CORE_WARN("Cannot add parent as child!");
        return;
    }
    if (child_data.parent == *this) {
        SD_CORE_WARN("This entity already has this child!");
        return;
    }

    Entity old_parent(child_data.parent, child.m_scene);
    if (old_parent) {
        old_parent.RemoveChild(child);
    }
    data.children.emplace(child);
    child_data.parent = *this;
    child_data.UpdateLocalPosition();
    child_data.UpdateLocalRotation();
    child_data.UpdateLocalScale();
}

void Entity::RemoveChild(Entity &child)
{
    auto &children = GetComponent<TransformComponent>().children;
    auto iter = children.find(child);
    if (iter != children.end()) {
        children.erase(iter);
    }
    else {
        SD_CORE_WARN("Entity cannot find specified child!");
    }
    auto &child_data = child.GetComponent<TransformComponent>();
    child_data.parent = Entity();
    child_data.UpdateLocalPosition();
    child_data.UpdateLocalRotation();
    child_data.UpdateLocalScale();
}

Entity::operator bool() const
{
    return m_handle != entt::null && m_scene != nullptr;
}

Entity::operator entt::entity() const { return m_handle; }

bool Entity::operator!=(const Entity &other) const { return !(*this == other); }

bool Entity::operator==(const Entity &other) const
{
    return m_handle == other.m_handle && m_scene == other.m_scene;
}

bool Entity::operator<(const Entity &other) const
{
    return m_handle < other.m_handle;
}

}  // namespace SD
