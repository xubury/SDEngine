#ifndef SD_TERRAIN_HPP
#define SD_TERRAIN_HPP

#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

namespace SD {

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

    const std::vector<Vertex> &getVertices() const;
    std::vector<Vertex> &getVertices();

   private:
    Mesh m_mesh;
    Material m_material;
    int m_gridSize;
    int m_vertexCount;
    void computeNormal();
};

}  // namespace SD

#endif /* SD_TERRAIN_HPP */
