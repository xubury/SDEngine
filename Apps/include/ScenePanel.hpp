#ifndef SCENE_PANEL_HPP
#define SCENE_PANEL_HPP

#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "Common/Base.hpp"

class ScenePanel {
   public:
    ScenePanel();
    ScenePanel(const sd::Ref<sd::Scene> &scene);

    void setScene(const sd::Ref<sd::Scene> &scene);

    void onImGui();

   private:
    void drawEntityNode(sd::Entity &entity);

    void drawComponents(sd::Entity &entity);

    sd::Ref<sd::Scene> m_scene;
    sd::Entity m_selectedEntity;
    sd::Entity m_destroyEntity;
};

#endif /* SCENE_PANEL_HPP */
