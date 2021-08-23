#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "ECS/RenderSystem.hpp"
#include "ECS/TerrainSystem.hpp"

namespace sd {

class RenderEngine {
   public:
    void init();

    RenderSystem *getRenderSystem();
    TerrainSystem *getTerrainSystem();

    void tick(float dt);
    void render();

    void setScene(Scene *scene);

   private:
    SystemManager m_systems;
    RenderSystem *m_renderSystem;
    TerrainSystem *m_terrainSystem;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
