#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "Core/System.hpp"
#include "Graphics/Viewport.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

#include "Asset/TextureAsset.hpp"

#include "TileBrush.hpp"

namespace SD {

class TileMapSystem : public System {
    enum Operation { NONE, ADD_ENTITY, REMOVE_ENTITY };

   public:
    TileMapSystem();
    void OnPush() override;
    void OnPop() override;
    void OnImGui() override;
    void OnRender() override;

   private:
    void ManipulateScene();
    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    // Selected sprite and its uvs
    TextureAsset *m_texture_asset;
    std::array<glm::vec2, 2> m_uvs{glm::vec2{0, 0}, glm::vec2{0, 0}};
    int m_priority;

    bool m_draw_outline;

    Operation m_operation;

    HandlerRegistration m_viewport_size_handler;
    HandlerRegistration m_viewport_pos_handler;
    HandlerRegistration m_viewport_state_handler;
    Viewport m_viewport;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
