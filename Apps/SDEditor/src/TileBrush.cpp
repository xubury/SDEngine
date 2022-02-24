#include "TileBrush.hpp"

namespace SD {

glm::ivec2 TileBrush::WorldToTile(const glm::vec3 &world,
                                  const Transform *transform) const {
    glm::vec2 tile;
    if (transform) {
        const glm::vec3 local = transform->WorldToLocal(world);

        tile.x = std::round(local.x / tile_size.x);
        tile.y = std::round(local.y / tile_size.y);
    } else {
        tile.x = std::round(world.x / tile_size.x);
        tile.y = std::round(world.y / tile_size.y);
    }
    return tile;
}

glm::vec3 TileBrush::TileToWorld(const glm::ivec2 &tile,
                                 const Transform *transform) const {
    glm::vec3 world(0);
    world.x = tile.x * tile_size.x;
    world.y = tile.y * tile_size.y;
    if (transform) {
        world = transform->LocalToWorld(world);
    }
    return world;
}

}  // namespace SD
