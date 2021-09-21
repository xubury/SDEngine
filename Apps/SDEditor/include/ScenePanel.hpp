#ifndef SCENE_PANEL_HPP
#define SCENE_PANEL_HPP

#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "Common/Base.hpp"
#include "ImGui/FileDialog.hpp"

class ScenePanel {
   public:
    ScenePanel();
    ScenePanel(sd::Scene *scene);

    void setScene(sd::Scene *scene);

    void setSelectedEntity(uint32_t entityId);

    sd::Entity getSelectedEntity() const;

    void onImGui();

    int getGizmoMode() const;
    int getGizmoType() const;

   private:
    void drawEntityNode(sd::Entity entity);

    void drawComponents(sd::Entity entity);

    sd::Scene *m_scene;
    sd::Entity m_selectedEntity;
    sd::Entity m_destroyEntity;
    bool m_fileDialogOpen = false;
    ImFileDialogInfo m_fileDialogInfo;

    int m_gizmoMode;
    int m_gizmoType;
};

#endif /* SCENE_PANEL_HPP */
