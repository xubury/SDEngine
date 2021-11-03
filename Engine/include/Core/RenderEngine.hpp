#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "Core/Layer.hpp"
#include "System/RenderSystem.hpp"
#include "System/TerrainSystem.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

enum GeometryBufferType {
    G_POSITION = 0,
    G_NORMAL,
    G_ALBEDO,
    G_AMBIENT,
    G_EMISSIVE,
    G_ENTITY_ID,
    G_DEPTH,
    GBUFFER_COUNT
};

class RenderEngine : public Layer, public SystemManager {
   public:
    RenderEngine(int width, int height, int samples);

    void onAttach() override;
    void onDetech() override;

    void onTick(float dt) override;
    void onRender() override;
    void onEventProcess(const SDL_Event &) override;

    RenderSystem *getRenderSystem();
    TerrainSystem *getTerrainSystem();

    void resize(int width, int height);

    void setRenderTarget(const Ref<RenderTarget> &target);
    const RenderTarget &getRenderTarget() const;

    void setScene(Scene *scene) { m_scene = scene; }
    Scene *getScene() const { return m_scene; };

    void setCamera(Camera *camera) { m_camera = camera; }
    Camera *getCamera() const { return m_camera; };

    void setExposure(float exposure) { m_exposure = exposure; }
    float getExposure() const { return m_exposure; }

    void setBloom(bool isBloom) { m_isBloom = isBloom; }
    bool getBloom() const { return m_isBloom; }

    void setBloomFactor(float bloom) { m_bloom = bloom; }
    float getBloomFactor() const { return m_bloom; }

   private:
    int m_width;
    int m_height;
    int m_samples;

    RenderTarget m_target;

    Ref<RenderSystem> m_renderSystem;
    Ref<TerrainSystem> m_terrainSystem;

    Scene *m_scene;
    Camera *m_camera;

    float m_exposure;

    float m_bloom;
    bool m_isBloom;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
