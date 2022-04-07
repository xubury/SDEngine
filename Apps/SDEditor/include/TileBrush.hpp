#ifndef SD_TILE_BRUSH_HPP
#define SD_TILE_BRUSH_HPP

#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"

#include <unordered_map>

namespace SD {

class TileBrush {
   private:
    Math::Ray m_ray;

    Vector3f m_world_pos;

   public:
    Vector2i count{1, 1};
    Vector2i pivot{0, 0};
    Vector4f color;
    Vector2i tile_size{10, 10};
    bool is_painting{false};

    Vector2i WorldToTile(const Vector3f &world, const Vector2i &grid_size,
                         const Transform *transform = nullptr) const;
    Vector3f TileToWorld(const Vector2i &tile, const Vector2i &grid_size,
                         const Transform *transform = nullptr) const;
    void SetRay(const Math::Ray &ray) { m_ray = ray; }

    bool CastRay()
    {
        Math::Plane plane(Vector3f(0, 0, 1), Vector3f(0));
        if (Math::IntersectRayPlane(m_ray, plane, m_world_pos)) {
            m_world_pos = Vector3f(WorldToTile(m_world_pos, tile_size), 0);
            m_world_pos.x -= pivot.x;
            m_world_pos.y += pivot.y;
            m_world_pos = TileToWorld(m_world_pos, tile_size);
            // Because quad's local coordinate is origin at center during
            // rendering , we need to at an offset to the pivot which
            // essentially moves the origin to the pivot.
            const Vector2f OFFSET = tile_size * (count - 1) / 2;
            m_world_pos.x += OFFSET.x;
            m_world_pos.y -= OFFSET.y;
            return true;
        }
        return false;
    };

    Vector3f GetSelectdPos() const { return m_world_pos; }

    Vector2i GetTileSize() const { return tile_size * count; }
};

}  // namespace SD

#endif  // !SD_TILE_BRUSH_HPP
