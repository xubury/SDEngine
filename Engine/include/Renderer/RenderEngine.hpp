#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "ECS/RenderSystem.hpp"
#include "ECS/TerrainSystem.hpp"

namespace sd {

class RenderEngine : public SystemManager {
   public:
    RenderEngine();

    void init(int width, int height, int samples);

    RenderSystem *getRenderSystem();
    TerrainSystem *getTerrainSystem();

    void resize(int width, int height);

    void setRenderTarget(RenderTarget *target);

    const RenderTarget *getRenderTarget() const;

    RenderTarget *getRenderTarget();

    void setCamera(Camera *camera);

    Camera *getCamera();

   private:
    RenderSystem *m_renderSystem;
    TerrainSystem *m_terrainSystem;
    RenderTarget *m_target;
    Camera *m_camera;
    RenderTarget m_defaultTarget;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
