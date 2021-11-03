#include "Core/RenderEngine.hpp"
#include "Graphics/Renderer.hpp"
#include "System/ProfileSystem.hpp"
#include "System/ShadowSystem.hpp"

namespace sd {

RenderEngine::RenderEngine(int width, int height, int samples)
    : Layer("Render Engine"),
      m_width(width),
      m_height(height),
      m_samples(samples),
      m_target(createRef<RenderTarget>(0, 0, width, height)),
      m_renderSystem(nullptr),
      m_terrainSystem(nullptr),
      m_scene(nullptr),
      m_camera(nullptr),
      m_exposure(1.5f),
      m_bloom(1.0f),
      m_isBloom(true) {
}

void RenderEngine::onAttach() {
    if (m_samples > 1) {
        Device::instance().enable(Operation::MULTISAMPLE);
    }
    addSystem<ShadowSystem>();
    m_renderSystem = addSystem<RenderSystem>(m_width, m_height, m_samples);
    addSystem<ProfileSystem>(m_width, m_height);
    m_terrainSystem = addSystem<TerrainSystem>();
}

void RenderEngine::onDetech() {}

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

RenderSystem *RenderEngine::getRenderSystem() { return m_renderSystem.get(); }

TerrainSystem *RenderEngine::getTerrainSystem() {
    return m_terrainSystem.get();
}

void RenderEngine::setRenderTarget(const Ref<RenderTarget> &target) {
    m_target = target;
}

const RenderTarget &RenderEngine::getRenderTarget() const { return *m_target; }

RenderTarget &RenderEngine::getRenderTarget() { return *m_target; }

void RenderEngine::resize(int width, int height) {
    if (m_camera) {
        m_camera->resize(width, height);
    }
    getRenderTarget().resize(width, height);
    dispatchEvent(SizeEvent(width, height));
}

}  // namespace sd
