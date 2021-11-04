#ifndef SD_TERRAIN_SYSTEM_HPP
#define SD_TERRAIN_SYSTEM_HPP

#include "System/System.hpp"
#include "System/Event.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Terrain.hpp"
#include "Utility/QuadTree.hpp"

namespace sd {

class TerrainSystem : public System {
   public:
    using TerrainGrid = std::vector<std::vector<Collidable>>;

   public:
    TerrainSystem();

    void onInit() override;

    void onDestroy() override;

    void onTick(float dt) override;

    void onRender() override;

    void onSceneEvent(const SceneEvent &event);

    void updateAllTerrains();

    void updateTerrain(entt::entity id);

   private:
    std::unordered_map<entt::entity, TerrainGrid> m_terrainGrids;
    QuadTree m_qtRoot;
    glm::vec3 m_min;
    glm::vec3 m_max;

    Scene *m_scene;
};

}  // namespace sd

#endif /* SD_TERRAIN_SYSTEM_HPP */
