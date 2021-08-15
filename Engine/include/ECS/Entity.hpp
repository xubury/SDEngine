#ifndef SD_ENTTY_HPP
#define SD_ENTTY_HPP

#include "Common/Export.hpp"
#include "Common/Assert.hpp"
#include "entt/entt.hpp"
#include "ECS/Scene.hpp"

namespace sd {

class SD_API Entity {
   public:
    static const uint32_t INVALID_ID;

   public:
    Entity();
    Entity(uint32_t handle, Scene *scene);
    Entity(entt::entity handle, Scene *scene);

    template <typename T, typename... Args>
    T &addComponent(Args &&...args);

    template <typename T>
    bool hasComponent();

    template <typename T>
    void removeComponent();

    template <typename T>
    T &getComponent();

    operator bool() const;
    operator entt::entity() const;
    operator uint32_t() const;

    bool operator==(const Entity &other) const;
    bool operator!=(const Entity &other) const;

    bool operator<(const Entity &other) const;

   private:
    entt::entity m_entityHandle;
    Scene *m_scene;
};

template <typename T, typename... Args>
T &Entity::addComponent(Args &&...args) {
    SD_CORE_ASSERT(!hasComponent<T>(), "Entity already has this component!");
    T &component = m_scene->m_registry.emplace<T>(m_entityHandle,
                                                  std::forward<Args>(args)...);
    m_scene->onComponentAdded<T>(*this, component);
    return component;
}

template <typename T>
bool Entity::hasComponent() {
    return m_scene->m_registry.all_of<T>(m_entityHandle);
}

template <typename T>
void Entity::removeComponent() {
    SD_CORE_ASSERT(hasComponent<T>(), "Entity does not have this component!");
    m_scene->m_registry.remove<T>(m_entityHandle);
}

template <typename T>
T &Entity::getComponent() {
    SD_CORE_ASSERT(hasComponent<T>(), "Entity does not have this component!");
    return m_scene->m_registry.get<T>(m_entityHandle);
}

}  // namespace sd

#endif /* SD_ENTTY_HPP */
