#ifndef SD_MESH_HPP
#define SD_MESH_HPP

#include "Renderer/Export.hpp"
#include "Graphics/VertexArray.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace SD {

struct SD_RENDERER_API Vertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bi_tangent;
};

class SD_RENDERER_API Mesh {
   public:
    Mesh();
    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<uint32_t> &indices);

    void Update();
    void Clear();

    void SetVerices(const std::vector<Vertex> &vertices);

    void SetIndices(const std::vector<uint32_t> &indices);

    VertexArray *GetVertexArray() const;

    void SetTopology(MeshTopology topology);
    MeshTopology GetTopology() const { return m_topology; }

    void SetPolygonMode(PolygonMode wireframe);
    PolygonMode GetPolygonMode() const;

    void SetMaterialIndex(int index) { m_materialId = index; }
    uint32_t GetMaterialIndex() const { return m_materialId; };

    const std::vector<Vertex> &GetVertices() const;
    std::vector<Vertex> &GetVertices();

   private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    Ref<VertexBuffer> m_vertexBuffer;
    Ref<IndexBuffer> m_indexBuffer;
    Ref<VertexArray> m_vertexArray;
    MeshTopology m_topology;
    uint32_t m_materialId;
    PolygonMode m_polygonMode;
};

}  // namespace SD

#endif /* SD_MESH_HPP */
