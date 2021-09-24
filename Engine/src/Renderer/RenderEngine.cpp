#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"

namespace sd {

RenderEngine::RenderEngine()
    : m_renderSystem(nullptr), m_terrainSystem(nullptr), m_target(nullptr) {}

void RenderEngine::init() {
    m_renderSystem = addSystem<RenderSystem>(this);
    m_terrainSystem = addSystem<TerrainSystem>();
}

RenderSystem *RenderEngine::getRenderSystem() { return m_renderSystem; }

TerrainSystem *RenderEngine::getTerrainSystem() { return m_terrainSystem; }

void RenderEngine::setRenderTarget(RenderTarget *target) {
    m_target = target;
    resize(target->getWidth(), target->getHeight());
}

RenderTarget *RenderEngine::getRenderTarget() const { return m_target; }

void RenderEngine::resize(int width, int height) {
    m_renderSystem->resize(width, height);
}

void RenderEngine::setCamera(Camera *camera) { m_camera = camera; }

Camera *RenderEngine::getCamera() { return m_camera; }

}  // namespace sd
