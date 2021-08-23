#include "ECS/Component.hpp"

namespace sd {

TerrainComponent::TerrainComponent() : terrain(1, 10) { computeBounds(); }

void TerrainComponent::computeBounds() {
    const auto &vertices = terrain.getVertices();
    uint32_t vertexCount = terrain.getVertexCount();
    objects.assign(vertexCount, std::vector<Collidable>(vertexCount - 1));
    for (uint32_t z = 0; z < vertexCount - 1; ++z) {
        for (uint32_t x = 0; x < vertexCount - 1; ++x) {
            glm::vec3 f00 = vertices[x + z * vertexCount].position;
            glm::vec3 f01 = vertices[x + 1 + z * vertexCount].position;
            glm::vec3 f10 = vertices[x + (z + 1) * vertexCount].position;
            glm::vec3 f11 = vertices[x + 1 + (z + 1) * vertexCount].position;
            glm::vec3 center = (f00 + f11) * 0.5f;
            glm::vec3 size = f11 - f00;
            float heightMin = std::min({f00.y, f01.y, f10.y, f11.y});
            float heightMax = std::max({f00.y, f01.y, f10.y, f11.y});
            objects[x][z] =
                Collidable(Rect(center.x, center.z, size.x, size.z),
                           std::pair<float, float>(heightMin, heightMax));
        }
    }
}

}  // namespace sd
