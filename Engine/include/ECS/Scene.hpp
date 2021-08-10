#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Common/Export.hpp"
#include "entt.hpp"

namespace sd {

class Entity;

class SD_API Scene {
   public:
    Scene() = default;
    ~Scene() = default;

    Entity createEntity(const std::string &name = "Entity");
    void destroyEntity(const Entity &entity);

    template <typename T>
    void onComponentAdded(const Entity &entity, T &component);

   private:
    friend class Entity;
    entt::registry m_registry;
};

template <typename T>
void Scene::onComponentAdded(const Entity &, T &) {}

}  // namespace sd

#endif /* SD_SCENE_HPP */
