#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "Core/ECSSystem.hpp"
#include "Graphics/Viewport.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

#include "Asset/TextureAsset.hpp"

#include "TileBrush.hpp"

namespace SD {

class TileMapSystem : public ECSSystem {
    enum class Operation { None, AddEntity, RemoveEntity };

   public:
    TileMapSystem();
    void OnImGui() override;
    void OnRender() override;

    void ManipulateScene(Framebuffer *framebuffer, Camera *camera);

   private:
    void SetViewport(float left, float top, float width, float height);
    Framebuffer *m_framebuffer;
    Camera *m_camera;
    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    // Selected sprite and its uvs
    ResourceId m_texture_id;
    std::array<Vector2f, 2> m_uvs{Vector2f{0, 0}, Vector2f{0, 0}};
    int m_priority;

    bool m_draw_outline;

    Operation m_operation;

    Viewport m_viewport;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
