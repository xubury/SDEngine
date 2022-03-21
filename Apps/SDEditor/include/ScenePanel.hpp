#ifndef SD_SCENE_PANEL_HPP
#define SD_SCENE_PANEL_HPP

#include "Utility/Base.hpp"
#include "Renderer/Event.hpp"
#include "Core/ECSSystem.hpp"
#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "ImGui/FileDialog.hpp"
#include "ECS/Component.hpp"
#include "ImGuizmo.h"

namespace SD {

class ScenePanel : public ECSSystem {
   public:
    ScenePanel();

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnSizeEvent(const RenderSizeEvent &event);

    void SetGizmoMode(ImGuizmo::MODE mode) { m_gizmo_mode = mode; }
    ImGuizmo::MODE GetGizmoMode() const;

    void SetGizmoOperation(ImGuizmo::OPERATION op) { m_gizmo_op = op; }
    ImGuizmo::OPERATION GetGizmoOperation() const;

    void Reset();

   private:
    void DrawEntityNode(Entity &entity);

    void DrawComponents(Entity &entity);

    void DrawMaterialsList(const std::vector<Material> &materials,
                           int *selected);
    void DrawAnimList(const std::vector<FrameAnimation<SpriteFrame>> &anims,
                      int *selected);

    Entity m_selected_entity;
    Entity m_entity_to_destroy;
    std::unordered_map<entt::entity, int> m_selected_material_id_map;
    std::unordered_map<entt::entity, int> m_selected_anim_id_map;

    ImGuizmo::MODE m_gizmo_mode;
    ImGuizmo::OPERATION m_gizmo_op;

    uint32_t m_width;
    uint32_t m_height;

    HandlerRegistration m_size_handler;
    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif /* SCENE_PANEL_HPP */
