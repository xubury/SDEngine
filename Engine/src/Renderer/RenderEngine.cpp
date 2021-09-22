#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"

namespace sd {

void RenderEngine::init() {
    m_renderSystem = m_systems.addSystem<RenderSystem>();
    m_terrainSystem = m_systems.addSystem<TerrainSystem>();
}

RenderSystem *RenderEngine::getRenderSystem() { return m_renderSystem; }

TerrainSystem *RenderEngine::getTerrainSystem() { return m_terrainSystem; }

void RenderEngine::tick(float dt) { m_systems.tick(dt); }

void RenderEngine::render() { m_systems.render(); }

void RenderEngine::setScene(Scene *scene) { m_systems.setScene(scene); }

void RenderEngine::resize(int width, int height) {
    m_renderSystem->resize(width, height);
}

}  // namespace sd
