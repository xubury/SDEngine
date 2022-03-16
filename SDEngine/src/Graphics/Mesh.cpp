#include "Graphics/Mesh.hpp"

namespace SD {

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<uint32_t> &indices)
    : m_vertices(vertices),
      m_indices(indices),
      m_topology(MeshTopology::TRIANGLES),
      m_polygonMode(PolygonMode::FILL) {
    m_vertexBuffer = VertexBuffer::Create(m_vertices.data(),
                                          m_vertices.size() * sizeof(Vertex),
                                          BufferIOType::DYNAMIC);

    m_indexBuffer = IndexBuffer::Create(m_indices.data(), m_indices.size(),
                                        BufferIOType::DYNAMIC);

    m_vertexArray = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::FLOAT3);
    layout.Push(BufferLayoutType::FLOAT2);
    layout.Push(BufferLayoutType::FLOAT3);
    layout.Push(BufferLayoutType::FLOAT3);
    layout.Push(BufferLayoutType::FLOAT3);
    m_vertexArray->AddVertexBuffer(m_vertexBuffer, layout);
    m_vertexArray->SetIndexBuffer(m_indexBuffer);
}

void Mesh::SetVerices(const std::vector<Vertex> &vertices) {
    m_vertices = vertices;
}

void Mesh::SetIndices(const std::vector<uint32_t> &indices) {
    m_indices = indices;
}

void Mesh::Update() {
    m_indexBuffer->UpdateData(m_indices.data(),
                              m_indices.size() * sizeof(uint32_t));
    m_vertexBuffer->UpdateData(m_vertices.data(),
                               m_vertices.size() * sizeof(Vertex));
}

void Mesh::Clear() {
    m_vertices.clear();
    m_indices.clear();
}

VertexArray *Mesh::GetVertexArray() const { return m_vertexArray.get(); }

void Mesh::SetTopology(MeshTopology topology) { m_topology = topology; }

void Mesh::SetPolygonMode(PolygonMode polygonMode) {
    m_polygonMode = polygonMode;
}

PolygonMode Mesh::GetPolygonMode() const { return m_polygonMode; }

const std::vector<Vertex> &Mesh::GetVertices() const { return m_vertices; }

std::vector<Vertex> &Mesh::GetVertices() { return m_vertices; }

}  // namespace SD
