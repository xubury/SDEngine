#ifndef SD_TILE_LAYOUT_HPP
#define SD_TILE_LAYOUT_HPP

#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"

#include <glm/glm.hpp>
#include <unordered_map>

template <>
struct std::hash<glm::ivec2> {
    std::size_t operator()(const glm::ivec2 &vec) const {
        return hash<int>()(vec.x) ^ hash<int>()(vec.y);
    }
};

namespace SD {

template <typename TILE>
class TileLayout {
   public:
    TileLayout() : m_tile_size(25, 25), m_priority(0), m_visible(true) {}
    TileLayout(const glm::ivec2 &tile_size) : m_tile_size(tile_size) {}

    bool Add(const glm::ivec2 &pos, const TILE &tile) {
        return m_tiles.emplace(pos, tile).second;
    }
    TILE &Get(const glm::ivec2 &pos) { return m_tiles.at(pos); }
    bool Has(const glm::ivec2 &pos) const { return m_tiles.count(pos); }
    void Clear(const glm::ivec2 &pos) { m_tiles.erase(pos); }

    void SetTileSize(const glm::ivec2 &tile_size) { m_tile_size = tile_size; }
    const glm::ivec2 &GetTileSize() const { return m_tile_size; }

    const std::unordered_map<glm::ivec2, TILE> &GetTiles() const {
        return m_tiles;
    }

    glm::ivec2 MapWorldToTile(const glm::vec3 &world,
                              const Transform *transform = nullptr) const {
        glm::vec2 tile;
        if (transform) {
            const glm::vec3 local = transform->ToLocalSpace(world);

            tile.x = std::ceil(local.x / m_tile_size.x - 0.5f);
            tile.y = std::ceil(local.y / m_tile_size.y - 0.5f);
        } else {
            tile.x = std::ceil(world.x / m_tile_size.x - 0.5f);
            tile.y = std::ceil(world.y / m_tile_size.y - 0.5f);
        }
        return tile;
    }

    glm::vec3 MapTileToWorld(const glm::ivec2 &tile,
                             const Transform *transform = nullptr) const {
        glm::vec3 world(0);
        world.x = tile.x * m_tile_size.x;
        world.y = tile.y * m_tile_size.y;
        if (transform) {
            world = transform->ToWorldSpace(world);
        }
        return world;
    }

    void SetPriority(int priority) { m_priority = priority; }
    int GetPriority() const { return m_priority; }

    void SetVisible(bool visible) { m_visible = visible; }
    bool GetVisible() const { return m_visible; }

    SERIALIZE(m_tile_size, m_tiles, m_priority, m_visible)
   private:
    glm::ivec2 m_tile_size;
    std::unordered_map<glm::ivec2, TILE> m_tiles;
    int m_priority;
    bool m_visible;
};

}  // namespace SD

#endif  // !SD_TILE_LAYOUT_HPP
