#include "ECS/Entity.hpp"

namespace SD {

const entt::entity Entity::INVALID_ID = entt::entity(-1);

void SetContext(entt::meta_ctx ctx) { entt::meta_ctx::bind(ctx); }

Entity::Entity() : m_handle(entt::null), m_scene(nullptr) {}

Entity::Entity(entt::entity handle, Scene *scene)
    : m_handle(handle), m_scene(scene) {}

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
