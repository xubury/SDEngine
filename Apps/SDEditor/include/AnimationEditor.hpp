#ifndef SD_ANMIATION_EDITOR_HPP
#define SD_ANMIATION_EDITOR_HPP

#include "Core/System.hpp"
#include "ImGui/FileDialog.hpp"
#include "ImGui/ImGuiWidget.hpp"

#include "Asset/TextureAsset.hpp"

namespace SD {

class AnimationEditor : public System {
   public:
    AnimationEditor();

    void OnInit() override;
    void OnPush() override;
    void OnPop() override;
    void OnImGui() override;

   private:
    HandlerRegistration m_entity_handler;
    Entity m_selected_entity;

    ImFileDialogInfo m_dialog_info;
    bool m_is_dialog_open;

    TextureAsset *m_texture_asset;
    std::array<glm::vec2, 2> m_uvs;
    glm::ivec2 m_count;
    glm::ivec2 m_tile_size;

    int m_anim_index;
};

}  // namespace SD

#endif
