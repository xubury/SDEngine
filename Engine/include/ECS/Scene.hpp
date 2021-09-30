#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Utils/Export.hpp"
#include "Utils/Base.hpp"
#include "entt/entt.hpp"
#include "ECS/Component.hpp"

namespace sd {

class Entity;

class SD_API Scene : public entt::registry {
   public:
    Scene() = default;
    ~Scene() = default;

    Entity createEntity(const std::string &name = "Entity");
    void destroyEntity(Entity &entity, bool isRoot = true);

    void addChildToEntity(Entity &parent, Entity &child);

    void removeChildFromEntity(Entity &parent, Entity &child);

    void refresh();

    void load(const std::string &filePath);

    void save(const std::string &filePath);

    template <typename T>
    void onComponentAdded(Entity &entity, T &component);

    void onComponentAdded(Entity &entity, LightComponent &component);

   private:
    void refreshEntityChildTranforms(Entity &entity);
    void refreshEntityModel(Entity &entity);
    void refreshEntityLight(Entity &entity);
};

template <typename T>
void Scene::onComponentAdded(Entity &, T &) {}

}  // namespace sd

#endif /* SD_SCENE_HPP */
