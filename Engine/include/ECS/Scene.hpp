#ifndef SD_SCENE_HPP
#define SD_SCENE_HPP

#include "Utils/Export.hpp"
#include "Utils/Base.hpp"
#include "entt/entt.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

class Entity;

class SD_API Scene {
   public:
    Scene() = default;
    ~Scene() = default;

    Entity createEntity(const std::string &name = "Entity");
    void destroyEntity(Entity entity, bool isRoot = true);

    void addChildToEntity(Entity parent, Entity child);

    void removeChildFromEntity(Entity parent, Entity child);

    const entt::registry &getRegistry() const;

    entt::registry &getRegistry();

    void clear();

    void refresh();

   private:
    void addEntityChildTranforms(Entity entity);
    friend class Entity;
    entt::registry m_registry;
};

}  // namespace sd

#endif /* SD_SCENE_HPP */
