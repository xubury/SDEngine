#ifndef SD_SCENE_PANEL_HPP
#define SD_SCENE_PANEL_HPP

#include "Utility/Base.hpp"
#include "Core/Vars.hpp"
#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ImGui/FileDialog.hpp"
#include "ECS/Component.hpp"
#include "ImGuizmo.h"

namespace SD {

class ScenePanel {
   public:
    ScenePanel();
    ScenePanel(Scene *scene);

    void SetScene(Scene *scene);

    void SetSelectedEntity(Entity entity);

    const Entity &GetSelectedEntity() const;
    Entity &GetSelectedEntity();

    void OnImGui();

    void SetGizmoMode(ImGuizmo::MODE mode) { m_gizmo_mode = mode; }
    ImGuizmo::MODE GetGizmoMode() const;

    void SetGizmoOperation(ImGuizmo::OPERATION op) { m_gizmo_op = op; }
    ImGuizmo::OPERATION GetGizmoOperation() const;

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
    Entity m_selected_entity;
    Entity m_entity_to_destroy;
    std::unordered_map<entt::entity, int> m_selected_material_id_map;

    ImGuizmo::MODE m_gizmo_mode;
    ImGuizmo::OPERATION m_gizmo_op;
};

}  // namespace SD

#endif /* SCENE_PANEL_HPP */
