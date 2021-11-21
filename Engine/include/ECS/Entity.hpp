#ifndef SD_ENTTY_HPP
#define SD_ENTTY_HPP

#include "Utility/Base.hpp"
#include "entt/entt.hpp"
#include "ECS/Scene.hpp"

namespace SD {

class SD_API Entity {
   public:
    static const entt::entity INVALID_ID;

   public:
    Entity();
    Entity(entt::entity handle, Scene *scene);

    template <typename T, typename... Args>
    T &AddComponent(Args &&...args);

    template <typename T>
    bool HasComponent() const;

    template <typename T>
    void RemoveComponent();

    template <typename T>
    T &GetComponent();

    template <typename T>
    const T &GetComponent() const;

    operator bool() const;
    operator entt::entity() const;

    bool operator==(const Entity &other) const;
    bool operator!=(const Entity &other) const;

    bool operator<(const Entity &other) const;

   private:
    template <typename T>
    void OnComponentAdded(T &component);

    template <typename>
    void OnComponentAdded(CameraComponent &component);

    entt::entity m_entityHandle;
    Scene *m_scene;
};

template <typename T, typename... Args>
T &Entity::AddComponent(Args &&...args) {
    SD_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component!");
    T &component =
        m_scene->emplace<T>(m_entityHandle, std::forward<Args>(args)...);
    OnComponentAdded<T>(component);
    return component;
}

template <typename T>
bool Entity::HasComponent() const {
    return m_scene->all_of<T>(m_entityHandle);
}

template <typename T>
void Entity::RemoveComponent() {
    SD_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component!");
    m_scene->remove<T>(m_entityHandle);
}

template <typename T>
T &Entity::GetComponent() {
    SD_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component!");
    return m_scene->get<T>(m_entityHandle);
}

template <typename T>
const T &Entity::GetComponent() const {
    SD_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component!");
    return m_scene->get<T>(m_entityHandle);
}

template <typename T>
void Entity::OnComponentAdded(T &) {}

template <typename>
void Entity::OnComponentAdded(CameraComponent &component) {
    Transform *trans = &GetComponent<TransformComponent>().transform;
    component.camera.SetTransform(trans);
}
}  // namespace SD

#endif /* SD_ENTTY_HPP */
