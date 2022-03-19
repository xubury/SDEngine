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
    enum Operation { None, AddEntity, RemoveEntity };

   public:
    TileMapSystem(Framebuffer *framebuffer);
    void OnPush() override;
    void OnPop() override;
    void OnImGui() override;
    void OnRender() override;

    void SetViewport(float left, float top, float width, float height);
    void ManipulateScene();

   private:
    Framebuffer *m_framebuffer;
    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    // Selected sprite and its uvs
    ResourceId m_texture_id;
    std::array<glm::vec2, 2> m_uvs{glm::vec2{0, 0}, glm::vec2{0, 0}};
    int m_priority;

    bool m_draw_outline;

    Operation m_operation;

    Viewport m_viewport;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
