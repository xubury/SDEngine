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

    void setSelectedEntity(sd::EntityId entityId);

    sd::Entity getSelectedEntity() const;

    void onImGui();

    int getGizmoMode() const;
    int getGizmoType() const;

   private:
    void drawEntityNode(sd::Entity &entity);

    void drawComponents(sd::Entity &entity);

    void drawMaterials(const std::vector<sd::Material> &material,
                       const ImVec2 &size);

    sd::Scene *m_scene;
    sd::Entity m_selectedEntity;
    sd::Entity m_destroyEntity;

    int m_gizmoMode;
    int m_gizmoType;
};

#endif /* SCENE_PANEL_HPP */
