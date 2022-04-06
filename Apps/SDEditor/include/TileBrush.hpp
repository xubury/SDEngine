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

   public:
    Vector2i count{0, 0};
    Vector2i pivot{0, 0};
    Vector2i tile_size{10, 10};

    Vector2i WorldToTile(const Vector3f &world,
                         const Transform *transform = nullptr) const;
    Vector3f TileToWorld(const Vector2i &tile,
                         const Transform *transform = nullptr) const;
    void SetRay(const Math::Ray &ray) { m_ray = ray; }

    bool GetSelectTile(Vector3f &pos, Vector2f& size)
    {
        bool ret = false;

        Vector3f world;
        Math::Plane plane(Vector3f(0, 0, 1), Vector3f(0));
        if (Math::IntersectRayPlane(m_ray, plane, world)) {
            pos = Vector3f(WorldToTile(world), 0);
            pos.x -= pivot.x;
            pos.y += pivot.y;
            pos = TileToWorld(pos);
            // Because quad's local coordinate is origin at center during
            // rendering , we need to at an offset to the pivot which
            // essentially moves the origin to the pivot.
            const Vector2f OFFSET = tile_size * (count - 1) / 2;
            pos.x += OFFSET.x;
            pos.y -= OFFSET.y;
            size = tile_size * count;
            ret = true;
        }
        return ret;
    }
};

}  // namespace SD

#endif  // !SD_TILE_BRUSH_HPP
