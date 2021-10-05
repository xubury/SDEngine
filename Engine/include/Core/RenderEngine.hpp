#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "Core/Layer.hpp"
#include "System/RenderSystem.hpp"
#include "System/TerrainSystem.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

class RenderEngine : public Layer {
   public:
    RenderEngine(int width, int height, int samples);

    void onEventProcess(const SDL_Event &) override;

    RenderSystem *getRenderSystem();
    TerrainSystem *getTerrainSystem();

    void resize(int width, int height);

    void setRenderTarget(const Ref<RenderTarget> &target);
    const RenderTarget &getRenderTarget() const;
    RenderTarget &getRenderTarget();

    void setCamera(Camera *camera);
    Camera *getCamera();

    float getExposure() const;
    void setExposure(float exposure);

    bool getBloom() const;
    void setBloom(bool isBloom);

    float getBloomFactor() const;
    void setBloomFactor(float Bloom);

   private:
    Ref<RenderTarget> m_target;

    RenderSystem *m_renderSystem;
    TerrainSystem *m_terrainSystem;
    Camera *m_camera;

    float m_exposure;

    float m_bloom;
    bool m_isBloom;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
