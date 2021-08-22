#include "ECS/Component.hpp"

namespace sd {

TerrainComponent::TerrainComponent() : terrain(1, 10) { computeBounds(); }

void TerrainComponent::computeBounds() {
    const auto &vertices = terrain.getVertices();
    uint32_t vertexCount = terrain.getVertexCount();
    aabb.assign(vertexCount, std::vector<AABB>(vertexCount - 1));
    for (uint32_t z = 0; z < vertexCount - 1; ++z) {
        for (uint32_t x = 0; x < vertexCount - 1; ++x) {
            glm::vec3 f00 = vertices[x + z * vertexCount].position;
            glm::vec3 f01 = vertices[x + 1 + z * vertexCount].position;
            glm::vec3 f10 = vertices[x + (z + 1) * vertexCount].position;
            glm::vec3 f11 = vertices[x + 1 + (z + 1) * vertexCount].position;
            aabb[x][z].updateBounds(f00);
            aabb[x][z].updateBounds(f01);
            aabb[x][z].updateBounds(f10);
            aabb[x][z].updateBounds(f11);
        }
    }
}

}  // namespace sd
