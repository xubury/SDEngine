#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Utility/Base.hpp"
#include "ECS/Export.hpp"
#include "ECS/Component.hpp"

#include "entt/entt.hpp"

namespace SD {

class Entity;

class SD_ECS_API Scene : public entt::registry {
   public:
    Scene();
    ~Scene() = default;

    Entity CreateEntity(const std::string &name = "Entity");
    void DestroyEntity(Entity &entity, bool isRoot = true);

    void AddChildToEntity(Entity &parent, Entity &child);

    void RemoveChildFromEntity(Entity &parent, Entity &child);

    void Refresh();

    void Load(const std::string &filePath);

    void Save(const std::string &filePath);

   private:
    void RefreshEntityChildTranforms(Entity &entity);
    void RefreshLight(Entity &entity);
};

}  // namespace SD

#endif /* SD_SCENE_HPP */
