#include "System/TerrainSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "ECS/Component.hpp"
#include <glm/gtx/extended_min_max.hpp>

namespace SD {

TerrainSystem::TerrainSystem()
    : System("Terrain"), m_min(FLT_MAX), m_max(-FLT_MAX) {}

void TerrainSystem::OnPush() {}

void TerrainSystem::OnPop() {}

void TerrainSystem::OnTick(float) {}

void TerrainSystem::OnRender() {}

void TerrainSystem::UpdateAllTerrains() {
    auto terrainView = scene->view<TransformComponent, TerrainComponent>();
    m_terrainGrids.clear();

    terrainView.each([this](const entt::entity &entity,
                            const TransformComponent &transformComp,
                            const TerrainComponent &terrainComp) {
        const auto &terrain = terrainComp.terrain;
        const auto &vertices = terrain.GetVertices();
        const uint32_t vertexCount = terrain.GetVertexCount();
        m_terrainGrids[entity].assign(vertexCount - 1,
                                      std::vector<Collidable>(vertexCount - 1));
        const Transform &transform = transformComp.transform;
        for (uint32_t z = 0; z < vertexCount - 1; ++z) {
            for (uint32_t x = 0; x < vertexCount - 1; ++x) {
                glm::vec3 f00 = transform.ToWorldSpace(
                    vertices[x + z * vertexCount].position);
                glm::vec3 f01 = transform.ToWorldSpace(
                    vertices[x + 1 + z * vertexCount].position);
                glm::vec3 f10 = transform.ToWorldSpace(
                    vertices[x + (z + 1) * vertexCount].position);
                glm::vec3 f11 = transform.ToWorldSpace(
                    vertices[x + 1 + (z + 1) * vertexCount].position);
                glm::vec3 center = (f00 + f11) * 0.5f;
                glm::vec3 size = f11 - f00;
                float minHeight = std::min({f00.y, f01.y, f10.y, f11.y});
                float maxHeight = std::max({f00.y, f01.y, f10.y, f11.y});
                m_min = glm::min(f00, f01, f10, f11);
                m_max = glm::max(f00, f01, f10, f11);
                m_terrainGrids[entity][x][z] =
                    Collidable(Rect(center.x, center.z, size.x, size.z),
                               std::pair<float, float>(minHeight, maxHeight));
            }
        }
    });
    m_qtRoot = QuadTree(Rect(0, 0, m_max.x - m_min.x, m_max.z - m_min.z), 8, 4);
    for (auto &[id, grid] : m_terrainGrids) {
        for (auto &row : grid) {
            for (auto &obj : row) {
                m_qtRoot.Insert(&obj);
            }
        }
    }
}

void TerrainSystem::UpdateTerrain(entt::entity id) {
    Entity entity(id, scene.get());
    const auto &transformComp = entity.GetComponent<TransformComponent>();
    const auto &terrainComp = entity.GetComponent<TerrainComponent>();
    const auto &terrain = terrainComp.terrain;
    const auto &vertices = terrain.GetVertices();
    uint32_t vertexCount = terrain.GetVertexCount();
    m_terrainGrids[id].assign(vertexCount,
                              std::vector<Collidable>(vertexCount - 1));
    const Transform &transform = transformComp.transform;
    for (uint32_t z = 0; z < vertexCount - 1; ++z) {
        for (uint32_t x = 0; x < vertexCount - 1; ++x) {
            glm::vec3 f00 =
                transform.ToWorldSpace(vertices[x + z * vertexCount].position);
            glm::vec3 f01 = transform.ToWorldSpace(
                vertices[x + 1 + z * vertexCount].position);
            glm::vec3 f10 = transform.ToWorldSpace(
                vertices[x + (z + 1) * vertexCount].position);
            glm::vec3 f11 = transform.ToWorldSpace(
                vertices[x + 1 + (z + 1) * vertexCount].position);
            glm::vec3 center = (f00 + f11) * 0.5f;
            glm::vec3 size = f11 - f00;
            float minHeight = std::min({f00.y, f01.y, f10.y, f11.y});
            float maxHeight = std::max({f00.y, f01.y, f10.y, f11.y});
            m_min = glm::min(f00, f01, f10, f11);
            m_max = glm::max(f00, f01, f10, f11);
            m_terrainGrids[id][x][z] =
                Collidable(Rect(center.x, center.z, size.x, size.z),
                           std::pair<float, float>(minHeight, maxHeight));
        }
    }
}

}  // namespace SD
