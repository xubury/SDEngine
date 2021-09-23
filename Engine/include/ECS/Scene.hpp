#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Utils/Export.hpp"
#include "Utils/Base.hpp"
#include "entt/entt.hpp"
#include "Graphics/Camera.hpp"

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

   private:
    void refreshEntityChildTranforms(Entity &entity);
    void refreshEntityModel(Entity &entity);
};

}  // namespace sd

#endif /* SD_SCENE_HPP */
