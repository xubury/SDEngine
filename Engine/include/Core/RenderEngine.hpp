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
    G_ENTITY_ID,
    GBUFFER_COUNT
};

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

    void setScene(Scene *scene);

   private:
    Ref<RenderTarget> m_target;

    RenderSystem *m_renderSystem;
    TerrainSystem *m_terrainSystem;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
