#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "TileMap/TileMap.hpp"

namespace SD {

class SD_SYSTEM_API TileMapSystem : public System {
   public:
    TileMapSystem(int width, int height);

    void OnTick(float dt) override;

    void OnPush() override;
    void OnPop() override;
    void OnRender() override;

    void SetCoordinate(const glm::vec2 &pos);

   private:
    TileMap m_tile_map;

    Camera m_camera;

    HandlerRegistration m_size_handler;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
