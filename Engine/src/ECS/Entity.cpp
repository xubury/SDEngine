#include "ECS/Entity.hpp"

namespace SD {

const entt::entity Entity::INVALID_ID = entt::entity(-1);

Entity::Entity() : m_entityHandle(entt::null), m_scene(nullptr) {}

Entity::Entity(entt::entity handle, Scene *scene)
    : m_entityHandle(handle), m_scene(scene) {}

Entity::operator bool() const { return m_entityHandle != entt::null; }

Entity::operator entt::entity() const { return m_entityHandle; }

bool Entity::operator!=(const Entity &other) const { return !(*this == other); }

bool Entity::operator==(const Entity &other) const {
    return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
}

bool Entity::operator<(const Entity &other) const {
    return m_entityHandle < other.m_entityHandle;
}

}  // namespace SD
