#ifndef SD_TERRAIN_HPP
#define SD_TERRAIN_HPP

#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

namespace sd {

class SD_API Terrain {
   public:
    Terrain(int gridSize, int vertexCount);

    void generateMesh();

    float height(float localX, float localZ) const;

    const Mesh &getMesh() const;

    const Material &getMaterial() const;

    int getGridSize() const;
    int getVertexCount() const;

    void setGridSize(int gridSize);
    void setVertexCount(int vertexCount);
   private:
    Mesh m_mesh;
    Material m_material;
    std::vector<Vertex> m_vertices;
    int m_gridSize;
    int m_vertexCount;
    void computeNormal();
};

}  // namespace sd

#endif /* TERRAIN_HPP */
