#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "Core/Layer.hpp"
#include "System/RenderSystem.hpp"
#include "System/TerrainSystem.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

class RenderEngine : public Layer, public SystemManager {
   public:
    RenderEngine(int width, int height, int samples);

    void onTick(float dt) override;
    void onRender() override;
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

    Ref<RenderSystem> m_renderSystem;
    Ref<TerrainSystem> m_terrainSystem;
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
