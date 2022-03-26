#ifndef SD_ANMIATION_EDITOR_HPP
#define SD_ANMIATION_EDITOR_HPP

#include "Core/ECSSystem.hpp"
#include "ImGui/FileDialog.hpp"
#include "ImGui/ImGuiWidget.hpp"

#include "Asset/TextureAsset.hpp"

namespace SD {

class AnimationEditor : public ECSSystem {
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

    ResourceId m_texture_id;
    std::array<Vector2f, 2> m_uvs;
    Vector2i m_count;
    Vector2i m_tile_size;

    int m_anim_index;
};

}  // namespace SD

#endif
