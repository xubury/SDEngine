#ifndef SD_MESH_HPP
#define SD_MESH_HPP

#include "Graphics/Export.hpp"
#include "Graphics/VertexArray.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace SD {

struct SD_GRAPHICS_API Vertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bi_tangent;
};

class SD_GRAPHICS_API Mesh {
   public:
    Mesh(const std::vector<Vertex> &vertices,
               const std::vector<uint32_t> &indices);

    void Update();
    void Clear();

    void SetVerices(const std::vector<Vertex> &vertices);

    void SetIndices(const std::vector<uint32_t> &indices);

    VertexBuffer *GetVertexBuffer() const;
    IndexBuffer *GetIndexBuffer() const;

    void SetTopology(MeshTopology topology);
    MeshTopology GetTopology() const { return m_topology; }

    void SetPolygonMode(PolygonMode wireframe);
    PolygonMode GetPolygonMode() const;

    const std::vector<Vertex> &GetVertices() const;
    std::vector<Vertex> &GetVertices();

   private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    Ref<VertexBuffer> m_vertexBuffer;
    Ref<IndexBuffer> m_indexBuffer;
    MeshTopology m_topology;
    PolygonMode m_polygonMode;
};

}  // namespace SD

#endif /* SD_MESH_HPP */
