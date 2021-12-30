#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "TileMap/Tile.hpp"
#include "Renderer/Sprite.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

namespace SD {

class SD_SYSTEM_API TileBrush {
   public:
    using UVGrids = std::vector<std::vector<std::array<glm::vec2, 2>>>;
    ResourceId sprite_id;
    std::array<glm::vec2, 2> uvs{glm::vec2{0, 0}, glm::vec2{0, 0}};
    glm::ivec2 count{0, 0};
    UVGrids ComputeTileUV() {
        float x_step = (uvs[1].x - uvs[0].x) / count.x;
        float y_step = (uvs[1].y - uvs[0].y) / count.y;
        UVGrids ret(count.y, std::vector<std::array<glm::vec2, 2>>(count.x));
        for (int y = 0; y < count.y; ++y) {
            for (int x = 0; x < count.x; ++x) {
                ret[y][x][0] = uvs[0] + glm::vec2(x * x_step, y * y_step);
                ret[y][x][1] =
                    uvs[0] + glm::vec2((x + 1) * x_step, (y + 1) * y_step);
            }
        }
        return ret;
    }
};

class SD_SYSTEM_API TileMapSystem : public System {
    enum Operation { NONE, ADD_ENTITY, REMOVE_ENTITY };

   public:
    TileMapSystem();

    void OnTick(float dt) override;

    void OnPush() override;
    void OnPop() override;

    void OnImGui() override;
    void OnRender() override;

    void SetViewport(const Viewport *viewport) { m_viewport = viewport; }

   private:
    void ApplyAction(TileLayout<Tile> &layout);

    const Viewport *m_viewport;

    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    glm::ivec2 m_select_tile_pos;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    bool m_draw_outline;

    Operation m_operation;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
