#ifndef SD_TILE_MAP_HPP
#define SD_TILE_MAP_HPP

#include "TileMap/Export.hpp"
#include "TileMap/TileLayout.hpp"

#include "Graphics/Texture.hpp"

#include "Utility/ResourceId.hpp"

namespace SD {

class SD_TILE_MAP_API TileMap {
   public:
    TileMap() = default;

    void SetTexture(const Ref<Texture> &texture);

    const Ref<Texture> GetTexture() const { return m_map; }

    TileLayout<std::array<glm::vec2, 2>> &GetLayout() { return m_layout; }
    const TileLayout<std::array<glm::vec2, 2>> &GetLayout() const {
        return m_layout;
    }

    SERIALIZE(m_layout)
   private:
    Ref<Texture> m_map;
    TileLayout<std::array<glm::vec2, 2>> m_layout;
};

};  // namespace SD

#endif  // !SD_TILE_MAP_HPP
