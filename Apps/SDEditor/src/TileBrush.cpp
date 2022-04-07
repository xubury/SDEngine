#include "TileBrush.hpp"

namespace SD {

Vector2i TileBrush::WorldToTile(const Vector3f &world,
                                const Vector2i &grid_size,
                                const Transform *transform) const
{
    Vector2i tile;
    if (transform) {
        const Vector3f local = transform->WorldToLocal(world);

        tile.x = std::round(local.x / grid_size.x);
        tile.y = std::round(local.y / grid_size.y);
    }
    else {
        tile.x = std::round(world.x / grid_size.x);
        tile.y = std::round(world.y / grid_size.y);
    }
    return tile;
}

Vector3f TileBrush::TileToWorld(const Vector2i &tile, const Vector2i &grid_size,
                                const Transform *transform) const
{
    Vector3f world(0);
    world.x = tile.x * grid_size.x;
    world.y = tile.y * grid_size.y;
    if (transform) {
        world = transform->LocalToWorld(world);
    }
    return world;
}

}  // namespace SD
