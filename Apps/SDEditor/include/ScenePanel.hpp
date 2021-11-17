#ifndef SD_SCENE_PANEL_HPP
#define SD_SCENE_PANEL_HPP

#include "Utility/Base.hpp"
#include "Core/Vars.hpp"
#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ImGui/FileDialog.hpp"
#include "ECS/Component.hpp"

namespace SD {

class ScenePanel {
   public:
    ScenePanel();
    ScenePanel(Scene *scene);

    void SetScene(Scene *scene);

    void SetSelectedEntity(Entity entity);

    Entity GetSelectedEntity() const;

    void OnImGui();

    int GetGizmoMode() const;
    int GetGizmoType() const;

    SET_APP_VARS;

   protected:
    APP_VARS
    MAKE_APP_VARS;

   private:
    void DrawEntityNode(Entity &entity);

    void DrawComponents(Entity &entity);

    void DrawMaterialsList(const std::vector<Material> &materials,
                           const ImVec2 &size, int *selected);

    void Reset();

    Scene *m_scene;
    Entity m_selectedEntity;
    Entity m_destroyEntity;
    std::unordered_map<entt::entity, int> m_selectedMaterialIdMap;

    int m_gizmoMode;
    int m_gizmoType;
};

}  // namespace SD

#endif /* SCENE_PANEL_HPP */
