#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "ECS/RenderSystem.hpp"
#include "ECS/TerrainSystem.hpp"

namespace sd {

class RenderEngine : public SystemManager {
   public:
    void init();

    RenderSystem *getRenderSystem();
    TerrainSystem *getTerrainSystem();

    void resize(int width, int height);

   private:
    RenderSystem *m_renderSystem;
    TerrainSystem *m_terrainSystem;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
