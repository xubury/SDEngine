#ifndef SD_MESH_HPP
#define SD_MESH_HPP

#include "Graphics/Export.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Material.hpp"
#include "Utility/Math.hpp"

#include <vector>

namespace SD {

struct SD_GRAPHICS_API Vertex {
    Vector3f position;
    Vector2f uv;
    Vector3f normal;
    Vector3f tangent;
    Vector3f bi_tangent;
};

class SD_GRAPHICS_API Mesh {
   public:
    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<uint32_t> &indices, MeshTopology topology);

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
    MeshTopology m_topology;
    Ref<VertexBuffer> m_vertexBuffer;
    Ref<IndexBuffer> m_indexBuffer;
    PolygonMode m_polygonMode;
};

}  // namespace SD

#endif /* SD_MESH_HPP */
