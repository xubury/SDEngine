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

    float getExposure() const;
    void setExposure(float exposure);

    bool getBloom() const;
    void setBloom(bool isBloom);

    float getBloomFactor() const;
    void setBloomFactor(float Bloom);

   private:
    RenderSystem *m_renderSystem;
    TerrainSystem *m_terrainSystem;
    RenderTarget *m_target;
    Camera *m_camera;
    RenderTarget m_defaultTarget;

    float m_exposure;

    float m_bloom;
    bool m_isBloom;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
