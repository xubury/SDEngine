#ifndef SCENE_PANEL_HPP
#define SCENE_PANEL_HPP

#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "Utility/Base.hpp"
#include "ImGui/FileDialog.hpp"
#include "ECS/Component.hpp"

class ScenePanel {
   public:
    ScenePanel();
    ScenePanel(sd::Scene *scene);

    void setScene(sd::Scene *scene);

    void setSelectedEntity(sd::Entity entity);

    sd::Entity getSelectedEntity() const;

    void onImGui();

    int getGizmoMode() const;
    int getGizmoType() const;

   private:
    void drawEntityNode(sd::Entity &entity);

    void drawComponents(sd::Entity &entity);

    void drawMaterialsList(const std::vector<sd::Material> &materials,
                           const ImVec2 &size, int *selected);

    void reset();

    sd::Scene *m_scene;
    sd::Entity m_selectedEntity;
    sd::Entity m_destroyEntity;
    std::unordered_map<entt::entity, int> m_selectedMaterialIdMap;

    int m_gizmoMode;
    int m_gizmoType;
};

#endif /* SCENE_PANEL_HPP */
