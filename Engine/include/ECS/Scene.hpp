#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Common/Export.hpp"
#include "Common/Base.hpp"
#include "entt/entt.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

class Entity;

class SD_API Scene {
   public:
    Scene() = default;
    ~Scene() = default;

    Entity createEntity(const std::string &name = "Entity");
    void destroyEntity(Entity &entity, bool isRoot = true);

    void addChildToEntity(Entity &parent, Entity &child);

    void removeChildFromEntity(Entity &parent, Entity &child);

    template <typename T>
    void onComponentAdded(const Entity &entity, T &component);

    const entt::registry &getRegistry() const;

    entt::registry &getRegistry();

   private:
    friend class Entity;
    entt::registry m_registry;
};

template <typename T>
void Scene::onComponentAdded(const Entity &, T &) {}

}  // namespace sd

#endif /* SD_SCENE_HPP */
