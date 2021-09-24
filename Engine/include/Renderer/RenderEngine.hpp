#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "ECS/RenderSystem.hpp"
#include "ECS/TerrainSystem.hpp"

namespace sd {

class RenderEngine : public SystemManager {
   public:
    RenderEngine();

    void init();

    RenderSystem *getRenderSystem();
    TerrainSystem *getTerrainSystem();

    void resize(int width, int height);

    void setRenderTarget(RenderTarget *target);

    RenderTarget *getRenderTarget() const;

    void setCamera(Camera *camera);

    Camera *getCamera();

   private:
    RenderSystem *m_renderSystem;
    TerrainSystem *m_terrainSystem;
    RenderTarget *m_target;
    Camera *m_camera;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
