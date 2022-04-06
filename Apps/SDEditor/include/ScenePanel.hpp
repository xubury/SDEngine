#ifndef SD_SCENE_PANEL_HPP
#define SD_SCENE_PANEL_HPP

#include "Utility/Base.hpp"
#include "Utility/EventDispatcher.hpp"
#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ImGui/FileDialog.hpp"
#include "ECS/Component.hpp"
#include "ImGuizmo.h"

namespace SD {

class ScenePanel {
   public:
    ScenePanel(EventDispatcher *dispatcher);

    void ImGui(Scene *scene);

    void SetGizmoMode(ImGuizmo::MODE mode) { m_gizmo_mode = mode; }
    ImGuizmo::MODE GetGizmoMode() const;

    void SetGizmoOperation(ImGuizmo::OPERATION op) { m_gizmo_op = op; }
    ImGuizmo::OPERATION GetGizmoOperation() const;

    void Reset();

   private:
    void DrawEntityNode(Entity &entity);

    void DrawComponents(Entity &entity);

    void DrawMaterialsList(std::vector<Material> &materials, int *selected);
    void DrawAnimList(const std::vector<FrameAnimation<SpriteFrame>> &anims,
                      int *selected);

    EventDispatcher *m_dispatcher;
    Entity m_selected_entity;
    Entity m_entity_to_destroy;
    std::unordered_map<entt::entity, int> m_selected_material_id_map;
    std::unordered_map<entt::entity, int> m_selected_anim_id_map;

    ImGuizmo::MODE m_gizmo_mode;
    ImGuizmo::OPERATION m_gizmo_op;

    HandlerRegistration m_scene_handler;
    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif /* SCENE_PANEL_HPP */
