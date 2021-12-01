#ifndef SD_TERRAIN_HPP
#define SD_TERRAIN_HPP

#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

namespace SD {

class SD_RENDERER_API Terrain {
   public:
    Terrain(int gridSize, int vertexCount);

    void GenerateMesh();

    float Height(float localX, float localZ) const;

    const Mesh &GetMesh() const;

    const Material &GetMaterial() const;

    int GetGridSize() const;
    int GetVertexCount() const;

    void SetGridSize(int gridSize);
    void SetVertexCount(int vertexCount);

    const std::vector<Vertex> &GetVertices() const;
    std::vector<Vertex> &GetVertices();

   private:
    Mesh m_mesh;
    Material m_material;
    int m_gridSize;
    int m_vertexCount;
    void ComputeNormal();
};

}  // namespace SD

#endif /* SD_TERRAIN_HPP */
