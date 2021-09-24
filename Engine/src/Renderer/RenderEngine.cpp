#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"

namespace sd {

RenderEngine::RenderEngine()
    : m_renderSystem(nullptr), m_terrainSystem(nullptr), m_target(nullptr) {}

void RenderEngine::init(int width, int height) {
    m_renderSystem = addSystem<RenderSystem>(this);
    m_terrainSystem = addSystem<TerrainSystem>();
    resize(width, height);
}

RenderSystem *RenderEngine::getRenderSystem() { return m_renderSystem; }

TerrainSystem *RenderEngine::getTerrainSystem() { return m_terrainSystem; }

void RenderEngine::setRenderTarget(RenderTarget *target) { m_target = target; }

const RenderTarget *RenderEngine::getRenderTarget() const {
    return m_target ? m_target : &m_defaultTarget;
}

void RenderEngine::resize(int width, int height) {
    if (m_target) m_target->resize(width, height);
    m_renderSystem->resize(width, height);
    m_defaultTarget.resize(width, height);
}

void RenderEngine::setCamera(Camera *camera) { m_camera = camera; }

Camera *RenderEngine::getCamera() { return m_camera; }

}  // namespace sd
