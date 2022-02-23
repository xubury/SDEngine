#include "TileBrush.hpp"

namespace SD {

glm::ivec2 TileBrush::WorldToTile(const glm::vec3 &world,
                                  const Transform *transform) const {
    glm::vec2 tile;
    if (transform) {
        const glm::vec3 local = transform->WorldToLocal(world);

        tile.x = std::ceil(local.x / tile_size.x - 0.5f);
        tile.y = std::ceil(local.y / tile_size.y - 0.5f);
    } else {
        tile.x = std::ceil(world.x / tile_size.x - 0.5f);
        tile.y = std::ceil(world.y / tile_size.y - 0.5f);
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
