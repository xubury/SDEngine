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

    Entity CreateEntity(const std::string &name);

    void Refresh();

    void Load(const std::string &filePath);

    void Save(const std::string &filePath);

    void SetCamera(Camera *camera);

    Camera *GetCamera();

    void SetSelectedEntity(entt::entity entity) { m_selected_entity = entity; }
    Entity GetSelectedEntity();
    void ResetSelectedEntity();

   private:
    void RefreshEntityChildTranforms(Entity &entity);
    void RefreshLight(Entity &entity);
    Camera *m_camera;
    entt::entity m_selected_entity;
};

}  // namespace SD

#endif /* SD_SCENE_HPP */
