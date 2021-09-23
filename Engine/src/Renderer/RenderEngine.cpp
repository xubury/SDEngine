#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"

namespace sd {

void RenderEngine::init() {
    m_renderSystem = addSystem<RenderSystem>();
    m_terrainSystem = addSystem<TerrainSystem>();
}

RenderSystem *RenderEngine::getRenderSystem() { return m_renderSystem; }

TerrainSystem *RenderEngine::getTerrainSystem() { return m_terrainSystem; }

void RenderEngine::resize(int width, int height) {
    m_renderSystem->resize(width, height);
}

}  // namespace sd
