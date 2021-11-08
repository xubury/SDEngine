#ifndef SD_SCENE_PANEL_HPP
#define SD_SCENE_PANEL_HPP

#include "ECS/Scene.hpp"
#include "ECS/Entity.hpp"
#include "Utility/Base.hpp"
#include "ImGui/FileDialog.hpp"
#include "ECS/Component.hpp"

namespace sd {

class ScenePanel {
   public:
    ScenePanel();
    ScenePanel(Scene *scene);

    void setScene(Scene *scene);

    void setSelectedEntity(Entity entity);

    Entity getSelectedEntity() const;

    void onImGui();

    int getGizmoMode() const;
    int getGizmoType() const;

   private:
    void drawEntityNode(Entity &entity);

    void drawComponents(Entity &entity);

    void drawMaterialsList(const std::vector<Material> &materials,
                           const ImVec2 &size, int *selected);

    void reset();

    Scene *m_scene;
    Entity m_selectedEntity;
    Entity m_destroyEntity;
    std::unordered_map<entt::entity, int> m_selectedMaterialIdMap;

    int m_gizmoMode;
    int m_gizmoType;
};

}  // namespace sd

#endif /* SCENE_PANEL_HPP */
