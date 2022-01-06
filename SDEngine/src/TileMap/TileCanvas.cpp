#include "TileMap/TileCanvas.hpp"

namespace SD {
TileCanvas::TileCanvas() : m_tile_size(25, 25) {}

TileCanvas::TileCanvas(const glm::ivec2 &tile_size) : m_tile_size(tile_size) {}

glm::ivec2 TileCanvas::WorldToTile(const glm::vec3 &world,
                                   const Transform *transform) const {
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

glm::vec3 TileCanvas::TileToWorld(const glm::ivec2 &tile,
                                  const Transform *transform) const {
    glm::vec3 world(0);
    world.x = tile.x * m_tile_size.x;
    world.y = tile.y * m_tile_size.y;
    if (transform) {
        world = transform->LocalToWorld(world);
    }
    return world;
}

}  // namespace SD
