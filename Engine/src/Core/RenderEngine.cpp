#include "Core/RenderEngine.hpp"
#include "Graphics/Renderer.hpp"
#include "System/ProfileSystem.hpp"

namespace sd {

RenderEngine::RenderEngine(int width, int height, int samples)
    : Layer("Render Engine"),
      m_target(createRef<RenderTarget>(0, 0, width, height)),
      m_renderSystem(nullptr),
      m_terrainSystem(nullptr) {
    m_renderSystem = addSystem<RenderSystem>(width, height, samples).get();
    addSystem<ProfileSystem>(width, height);
    m_terrainSystem = addSystem<TerrainSystem>().get();
}

void RenderEngine::onTick(float dt) {
    for (auto &system : getSystems()) {
        system->onTick(dt);
    }
}

void RenderEngine::onRender() {
    for (auto &system : getSystems()) {
        system->onRender();
    }
}

void RenderEngine::onEventProcess(const SDL_Event &event) {
    if (event.type == SDL_WINDOWEVENT) {
        switch (event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                resize(event.window.data1, event.window.data2);
        }
    }
}

RenderSystem *RenderEngine::getRenderSystem() { return m_renderSystem; }

TerrainSystem *RenderEngine::getTerrainSystem() { return m_terrainSystem; }

void RenderEngine::setRenderTarget(const Ref<RenderTarget> &target) {
    m_target = target;
}

const RenderTarget &RenderEngine::getRenderTarget() const { return *m_target; }

RenderTarget &RenderEngine::getRenderTarget() { return *m_target; }

void RenderEngine::resize(int width, int height) {
    getRenderTarget().resize(width, height);
    dispatchEvent(SizeEvent(width, height));
}

void RenderEngine::setScene(Scene *scene) { dispatchEvent(SceneEvent(scene)); }

}  // namespace sd
