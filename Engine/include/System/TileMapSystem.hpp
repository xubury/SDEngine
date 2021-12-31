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
    using BrushOutput = std::vector<std::vector<Tile>>;
    ResourceId sprite_id;
    std::array<glm::vec2, 2> uvs{glm::vec2{0, 0}, glm::vec2{0, 0}};
    glm::ivec2 count{0, 0};
    glm::ivec2 pivot{0, 0};
    glm::ivec2 select_pos;
    void Output(Entity &parent) {
        if (!parent.HasComponent<TileLayoutComponent>()) {
            return;
        }
        auto &layout = parent.GetComponent<TileLayoutComponent>().layout;
        auto &transform = parent.GetComponent<TransformComponent>();
        float x_step = (uvs[1].x - uvs[0].x) / count.x;
        float y_step = (uvs[1].y - uvs[0].y) / count.y;
        for (int y = 0; y < count.y; ++y) {
            for (int x = 0; x < count.x; ++x) {
                glm::ivec2 pos(select_pos.x + (x - pivot.x),
                               select_pos.y - (y - pivot.y));
                if (layout.Has(pos)) {
                    Entity child(layout.Get(pos), parent.GetScene());
                    child.Destroy();
                    layout.Clear(pos);
                }
                Entity child = parent.CreateChild("Tile");
                layout.Add(pos, child);
                auto &comp = child.AddComponent<SpriteComponent>();
                child.GetComponent<TransformComponent>().SetWorldPosition(
                    layout.TileToGlobal(pos, &transform.GetWorldTransform()));
                comp.id = sprite_id;
                comp.uvs[0] = uvs[0] + glm::vec2(x * x_step, y * y_step);
                comp.uvs[1] =
                    uvs[0] + glm::vec2((x + 1) * x_step, (y + 1) * y_step);
                comp.size = layout.GetTileSize();
            }
        }
    }

    void Clear(Entity &parent) {
        if (!parent.HasComponent<TileLayoutComponent>()) {
            return;
        }
        auto &layout = parent.GetComponent<TileLayoutComponent>().layout;
        for (int y = 0; y < count.y; ++y) {
            for (int x = 0; x < count.x; ++x) {
                glm::ivec2 pos(select_pos.x + (x - pivot.x),
                               select_pos.y - (y - pivot.y));
                if (layout.Has(pos)) {
                    Entity child(layout.Get(pos), parent.GetScene());
                    child.Destroy();
                    layout.Clear(pos);
                }
            }
        }
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
    void ApplyAction(Entity &entity);

    const Viewport *m_viewport;

    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    bool m_draw_outline;

    Operation m_operation;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
