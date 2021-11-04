#ifndef SD_ENTTY_HPP
#define SD_ENTTY_HPP

#include "Utility/Export.hpp"
#include "Utility/Assert.hpp"
#include "entt/entt.hpp"
#include "ECS/Scene.hpp"

namespace sd {

class SD_API Entity {
   public:
    static const entt::entity INVALID_ID;

   public:
    Entity();
    Entity(entt::entity handle, Scene *scene);

    template <typename T, typename... Args>
    T &addComponent(Args &&...args);

    template <typename T>
    bool hasComponent() const;

    template <typename T>
    void removeComponent();

    template <typename T>
    T &getComponent();

    template <typename T>
    const T &getComponent() const;

    operator bool() const;
    operator entt::entity() const;

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
    T &component =
        m_scene->emplace<T>(m_entityHandle, std::forward<Args>(args)...);
    m_scene->onComponentAdded(*this, component);
    return component;
}

template <typename T>
bool Entity::hasComponent() const {
    return m_scene->all_of<T>(m_entityHandle);
}

template <typename T>
void Entity::removeComponent() {
    SD_CORE_ASSERT(hasComponent<T>(), "Entity does not have this component!");
    m_scene->remove<T>(m_entityHandle);
}

template <typename T>
T &Entity::getComponent() {
    SD_CORE_ASSERT(hasComponent<T>(), "Entity does not have this component!");
    return m_scene->get<T>(m_entityHandle);
}

template <typename T>
const T &Entity::getComponent() const {
    SD_CORE_ASSERT(hasComponent<T>(), "Entity does not have this component!");
    return m_scene->get<T>(m_entityHandle);
}

}  // namespace sd

#endif /* SD_ENTTY_HPP */
