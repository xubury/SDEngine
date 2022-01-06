#ifndef SD_TILE_CANVAS_HPP
#define SD_TILE_CANVAS_HPP

#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"

#include "TileMap/Export.hpp"

#include <glm/glm.hpp>
#include <unordered_map>

namespace SD {

class SD_TILE_MAP_API TileCanvas {
   public:
    TileCanvas();
    TileCanvas(const glm::ivec2 &tile_size);

    void SetTileSize(const glm::ivec2 &tile_size) { m_tile_size = tile_size; }
    const glm::ivec2 &GetTileSize() const { return m_tile_size; }

    glm::ivec2 WorldToTile(const glm::vec3 &world,
                           const Transform *transform = nullptr) const;
    glm::vec3 TileToWorld(const glm::ivec2 &tile,
                          const Transform *transform = nullptr) const;

   private:
    glm::ivec2 m_tile_size;
};

class TileBrush {
   private:
    Math::Ray m_ray;

   public:
    glm::ivec2 count{0, 0};
    glm::ivec2 pivot{0, 0};
    TileCanvas canvas;

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
};

}  // namespace SD

#endif  // !SD_TILE_CANVAS_HPP
