#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"

namespace sd {

RenderEngine::RenderEngine()
    : m_renderSystem(nullptr),
      m_terrainSystem(nullptr),
      m_target(nullptr),
      m_exposure(1.5f),
      m_bloom(1.0f),
      m_isBloom(true) {}

void RenderEngine::init(int width, int height, int samples) {
    m_renderSystem = addSystem<RenderSystem>(this, width, height, samples);
    m_terrainSystem = addSystem<TerrainSystem>();
    m_defaultTarget.resize(width, height);
}

RenderSystem *RenderEngine::getRenderSystem() { return m_renderSystem; }

TerrainSystem *RenderEngine::getTerrainSystem() { return m_terrainSystem; }

void RenderEngine::setRenderTarget(RenderTarget *target) { m_target = target; }

const RenderTarget *RenderEngine::getRenderTarget() const {
    return m_target ? m_target : &m_defaultTarget;
}

RenderTarget *RenderEngine::getRenderTarget() {
    return m_target ? m_target : &m_defaultTarget;
}
void RenderEngine::resize(int width, int height) {
    if (m_camera) m_camera->resize(width, height);
    getRenderTarget()->resize(width, height);
    m_renderSystem->resize(width, height);
}

void RenderEngine::setCamera(Camera *camera) { m_camera = camera; }

Camera *RenderEngine::getCamera() { return m_camera; }

void RenderEngine::setExposure(float exposure) { m_exposure = exposure; }

float RenderEngine::getExposure() const { return m_exposure; }

void RenderEngine::setBloom(bool isBloom) { m_isBloom = isBloom; }

bool RenderEngine::getBloom() const { return m_isBloom; }

void RenderEngine::setBloomFactor(float bloom) { m_bloom = bloom; }

float RenderEngine::getBloomFactor() const { return m_bloom; }

}  // namespace sd
