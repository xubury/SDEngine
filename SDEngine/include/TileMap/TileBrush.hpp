#ifndef SD_TILE_BRUSH_HPP
#define SD_TILE_BRUSH_HPP

#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"

#include "TileMap/Export.hpp"

#include <glm/glm.hpp>
#include <unordered_map>

namespace SD {

class TileBrush {
   private:
    Math::Ray m_ray;

   public:
    glm::ivec2 count{0, 0};
    glm::ivec2 pivot{0, 0};
    glm::ivec2 tile_size;

    glm::ivec2 WorldToTile(const glm::vec3 &world,
                           const Transform *transform = nullptr) const;
    glm::vec3 TileToWorld(const glm::ivec2 &tile,
                          const Transform *transform = nullptr) const;
    void SetRay(const Math::Ray &ray) { m_ray = ray; }

    bool GetSelectPos(glm::vec3 &pos) {
        bool ret = false;

        const glm::vec2 BRUSH_SIZE = tile_size * count;
        glm::vec3 offset(BRUSH_SIZE.x / 2 - tile_size.x / 2.f,
                         -BRUSH_SIZE.y / 2 + tile_size.y / 2.f, 0);
        glm::vec3 world;
        Math::Plane plane(glm::vec3(0, 0, 1), glm::vec3(0));
        if (Math::IntersectRayPlane(m_ray, plane, world)) {
            pos = glm::vec3(WorldToTile(world), 0);
            pos.x -= pivot.x;
            pos.y += pivot.y;
            pos = TileToWorld(pos) + offset;
            ret = true;
        }
        return ret;
    }
};

}  // namespace SD

#endif  // !SD_TILE_BRUSH_HPP
