#ifndef SD_TILE_CANVAS_HPP
#define SD_TILE_CANVAS_HPP

#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"
#include "ECS/Entity.hpp"

#include <glm/glm.hpp>
#include <unordered_map>

namespace SD {

class TileCanvas {
   public:
    TileCanvas() : m_tile_size(25, 25) {}
    TileCanvas(const glm::ivec2 &tile_size) : m_tile_size(tile_size) {}

    void SetTileSize(const glm::ivec2 &tile_size) { m_tile_size = tile_size; }
    const glm::ivec2 &GetTileSize() const { return m_tile_size; }

    glm::ivec2 WorldToTile(const glm::vec3 &world,
                           const Transform *transform = nullptr) const {
        glm::vec2 tile;
        if (transform) {
            const glm::vec3 local = transform->WorldToLocal(world);

            tile.x = std::ceil(local.x / m_tile_size.x - 0.5f);
            tile.y = std::ceil(local.y / m_tile_size.y - 0.5f);
        } else {
            tile.x = std::ceil(world.x / m_tile_size.x - 0.5f);
            tile.y = std::ceil(world.y / m_tile_size.y - 0.5f);
        }
        return tile;
    }

    glm::vec3 TileToWorld(const glm::ivec2 &tile,
                          const Transform *transform = nullptr) const {
        glm::vec3 world(0);
        world.x = tile.x * m_tile_size.x;
        world.y = tile.y * m_tile_size.y;
        if (transform) {
            world = transform->LocalToWorld(world);
        }
        return world;
    }

   private:
    glm::ivec2 m_tile_size;
};

class  TileBrush {
   private:
    Math::Ray m_ray;
    int m_priority{0};

   public:
    ResourceId sprite_id;
    std::array<glm::vec2, 2> uvs{glm::vec2{0, 0}, glm::vec2{0, 0}};
    glm::ivec2 count{0, 0};
    glm::ivec2 pivot{0, 0};
    TileCanvas canvas;
    Scene *scene;

    void SetPriority(int priority) { m_priority = priority; }
    int GetPriority() const { return m_priority; }

    void SetRay(const Math::Ray &ray) { m_ray = ray; }

    bool GetSelectPos(glm::vec3 &pos) {
        bool ret = false;
        const glm::ivec2 TILE_SIZE = canvas.GetTileSize();

        const glm::vec2 BRUSH_SIZE = TILE_SIZE * count;
        glm::vec3 offset(BRUSH_SIZE.x / 2 - TILE_SIZE.x / 2.f,
                         -BRUSH_SIZE.y / 2 + TILE_SIZE.y / 2.f, 0);
        glm::vec3 world;
        Math::Plane plane(glm::vec3(0, 0, 1), glm::vec3(0));
        if (Math::IntersectRayPlane(m_ray, plane, world)) {
            pos = glm::vec3(canvas.WorldToTile(world), 0);
            pos.x -= pivot.x;
            pos.y += pivot.y;
            pos = canvas.TileToWorld(pos) + offset;
            ret = true;
        }
        return ret;
    }

    void Output() {
        if (scene) {
            glm::vec3 world;
            if (GetSelectPos(world)) {
                Entity child = scene->CreateEntity("Tile");
                auto &comp = child.AddComponent<SpriteComponent>();
                child.GetComponent<TransformComponent>().SetWorldPosition(
                    world);
                comp.id = sprite_id;
                comp.uvs = uvs;
                comp.size = canvas.GetTileSize() * count;
                comp.priority = m_priority;
            }
        }
    }

    void Clear() {}
};

}  // namespace SD

#endif  // !SD_TILE_CANVAS_HPP
