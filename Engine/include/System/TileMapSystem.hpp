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
   private:
    Math::Ray m_ray;
    int m_priority{0};

   public:
    ResourceId sprite_id;
    std::array<glm::vec2, 2> uvs{glm::vec2{0, 0}, glm::vec2{0, 0}};
    glm::ivec2 count{0, 0};
    glm::ivec2 pivot{0, 0};
    Entity target;

    void SetPriority(int priority) { m_priority = priority; }
    int GetPriority() const { return m_priority; }

    void SetRay(const Math::Ray &ray) { m_ray = ray; }

    glm::ivec2 GetSelectTilePos() {
        glm::ivec2 pos;
        if (target && target.HasComponent<TileLayoutComponent>()) {
            glm::vec3 world;
            auto &transform = target.GetComponent<TransformComponent>();
            auto &layout = target.GetComponent<TileLayoutComponent>().layout;
            Math::Plane plane(transform.GetWorldFront(),
                              transform.GetWorldPosition());
            if (Math::IntersectRayPlane(m_ray, plane, world)) {
                pos = layout.WorldToTile(world, &transform.GetWorldTransform());
            }
        }
        pos.x -= pivot.x;
        pos.y += pivot.y;
        return pos;
    }

    void Output() {
        if (target && target.HasComponent<TileLayoutComponent>()) {
            auto &transform = target.GetComponent<TransformComponent>();
            auto &layout = target.GetComponent<TileLayoutComponent>().layout;
            Math::Plane plane(transform.GetWorldFront(),
                              transform.GetWorldPosition());
            glm::vec3 world;
            if (Math::IntersectRayPlane(m_ray, plane, world)) {
                glm::ivec2 select_pos =
                    layout.WorldToTile(world, &transform.GetWorldTransform());
                float x_step = (uvs[1].x - uvs[0].x) / count.x;
                float y_step = (uvs[1].y - uvs[0].y) / count.y;
                for (int y = 0; y < count.y; ++y) {
                    for (int x = 0; x < count.x; ++x) {
                        glm::ivec2 pos(select_pos.x + (x - pivot.x),
                                       select_pos.y - (y - pivot.y));
                        if (layout.Has(pos)) {
                            Entity child(layout.Get(pos), target.GetScene());
                            child.Destroy();
                            layout.Clear(pos);
                        }
                        Entity child = target.CreateChild("Tile");
                        layout.Add(pos, child);
                        auto &comp = child.AddComponent<SpriteComponent>();
                        child.GetComponent<TransformComponent>()
                            .SetWorldPosition(layout.TileToWorld(
                                pos, &transform.GetWorldTransform()));
                        comp.id = sprite_id;
                        comp.uvs[0] =
                            uvs[0] + glm::vec2(x * x_step, y * y_step);
                        comp.uvs[1] = uvs[0] + glm::vec2((x + 1) * x_step,
                                                         (y + 1) * y_step);
                        comp.size = layout.GetTileSize();
                        comp.priority = m_priority;
                    }
                }
            }
        }
    }

    void Clear() {
        if (target && target.HasComponent<TileLayoutComponent>()) {
            auto &transform = target.GetComponent<TransformComponent>();
            auto &layout = target.GetComponent<TileLayoutComponent>().layout;
            Math::Plane plane(transform.GetWorldFront(),
                              transform.GetWorldPosition());
            glm::vec3 world;
            if (Math::IntersectRayPlane(m_ray, plane, world)) {
                glm::ivec2 select_pos =
                    layout.WorldToTile(world, &transform.GetWorldTransform());
                for (int y = 0; y < count.y; ++y) {
                    for (int x = 0; x < count.x; ++x) {
                        glm::ivec2 pos(select_pos.x + (x - pivot.x),
                                       select_pos.y - (y - pivot.y));
                        if (layout.Has(pos)) {
                            Entity child(layout.Get(pos), target.GetScene());
                            child.Destroy();
                            layout.Clear(pos);
                        }
                    }
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
    const Viewport *m_viewport;

    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    bool m_draw_outline;

    Operation m_operation;

    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
