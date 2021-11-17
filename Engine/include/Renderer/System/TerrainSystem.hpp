#ifndef SD_TERRAIN_SYSTEM_HPP
#define SD_TERRAIN_SYSTEM_HPP

#include "Core/System.hpp"
#include "Renderer/Terrain.hpp"
#include "ECS/Entity.hpp"
#include "Utility/QuadTree.hpp"

namespace SD {

class TerrainSystem : public System {
   public:
    using TerrainGrid = std::vector<std::vector<Collidable>>;

   public:
    TerrainSystem();

    void OnPush() override;

    void OnPop() override;

    void OnTick(float dt) override;

    void OnRender() override;

    void UpdateAllTerrains();

    void UpdateTerrain(entt::entity id);

   private:
    std::unordered_map<entt::entity, TerrainGrid> m_terrainGrids;
    QuadTree m_qtRoot;
    glm::vec3 m_min;
    glm::vec3 m_max;
};

}  // namespace SD

#endif /* SD_TERRAIN_SYSTEM_HPP */
