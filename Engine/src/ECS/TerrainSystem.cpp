#include "ECS/TerrainSystem.hpp"
#include "ECS/Component.hpp"

namespace sd {

TerrainSystem::TerrainSystem() {}

void TerrainSystem::onTick(float) {}

void TerrainSystem::onRender() {}

void TerrainSystem::onSceneChange() { updateAllTerrains(); }

void TerrainSystem::updateAllTerrains() {
    auto terrainView =
        m_scene->getRegistry().view<TransformComponent, TerrainComponent>();
    m_terrainGrids.clear();

    terrainView.each([this](const entt::entity &entity,
                            const TransformComponent &transformComp,
                            const TerrainComponent &terrainComp) {
        const auto &terrain = terrainComp.terrain;
        const auto &vertices = terrain.getVertices();
        uint32_t vertexCount = terrain.getVertexCount();
        uint32_t id = static_cast<uint32_t>(entity);
        m_terrainGrids[id].assign(vertexCount,
                                  std::vector<Collidable>(vertexCount - 1));
        const Transform &transform = transformComp.transform;
        for (uint32_t z = 0; z < vertexCount - 1; ++z) {
            for (uint32_t x = 0; x < vertexCount - 1; ++x) {
                glm::vec3 f00 = transform.toWorldSpace(
                    vertices[x + z * vertexCount].position);
                glm::vec3 f01 = transform.toWorldSpace(
                    vertices[x + 1 + z * vertexCount].position);
                glm::vec3 f10 = transform.toWorldSpace(
                    vertices[x + (z + 1) * vertexCount].position);
                glm::vec3 f11 = transform.toWorldSpace(
                    vertices[x + 1 + (z + 1) * vertexCount].position);
                glm::vec3 center = (f00 + f11) * 0.5f;
                glm::vec3 size = f11 - f00;
                float heightMin = std::min({f00.y, f01.y, f10.y, f11.y});
                float heightMax = std::max({f00.y, f01.y, f10.y, f11.y});
                m_terrainGrids[id][x][z] =
                    Collidable(Rect(center.x, center.z, size.x, size.z),
                               std::pair<float, float>(heightMin, heightMax));
            }
        }
    });
}

void TerrainSystem::updateTerrain(uint32_t id) {
    Entity entity(id, m_scene);
    const auto &transformComp = entity.getComponent<TransformComponent>();
    const auto &terrainComp = entity.getComponent<TerrainComponent>();
    const auto &terrain = terrainComp.terrain;
    const auto &vertices = terrain.getVertices();
    uint32_t vertexCount = terrain.getVertexCount();
    m_terrainGrids[id].assign(vertexCount,
                              std::vector<Collidable>(vertexCount - 1));
    const Transform &transform = transformComp.transform;
    for (uint32_t z = 0; z < vertexCount - 1; ++z) {
        for (uint32_t x = 0; x < vertexCount - 1; ++x) {
            glm::vec3 f00 =
                transform.toWorldSpace(vertices[x + z * vertexCount].position);
            glm::vec3 f01 = transform.toWorldSpace(
                vertices[x + 1 + z * vertexCount].position);
            glm::vec3 f10 = transform.toWorldSpace(
                vertices[x + (z + 1) * vertexCount].position);
            glm::vec3 f11 = transform.toWorldSpace(
                vertices[x + 1 + (z + 1) * vertexCount].position);
            glm::vec3 center = (f00 + f11) * 0.5f;
            glm::vec3 size = f11 - f00;
            float heightMin = std::min({f00.y, f01.y, f10.y, f11.y});
            float heightMax = std::max({f00.y, f01.y, f10.y, f11.y});
            m_terrainGrids[id][x][z] =
                Collidable(Rect(center.x, center.z, size.x, size.z),
                           std::pair<float, float>(heightMin, heightMax));
        }
    }
}

}  // namespace sd
