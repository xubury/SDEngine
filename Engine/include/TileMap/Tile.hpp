#ifndef SD_TILE_HPP
#define SD_TILE_HPP

#include "Utility/ResourceId.hpp"

namespace SD {

class Tile {
   public:
    ResourceId sprite_id;
    std::array<glm::vec2, 2> uvs;

    SERIALIZE(sprite_id, uvs)
};

};  // namespace SD

#endif  // !SD_TILE_HPP
