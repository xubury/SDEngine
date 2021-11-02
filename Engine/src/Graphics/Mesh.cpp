#include "Graphics/Mesh.hpp"

namespace sd {

Mesh::Mesh()
    : m_vertices(0),
      m_indices(0),
      m_topology(MeshTopology::TRIANGLES),
      m_materialId(0),
      m_wireframe(false) {
    m_vertexBuffer =
        VertexBuffer::create(m_vertices.data(), m_vertices.size(),
                             sizeof(Vertex), BufferIOType::DYNAMIC);

    m_indexBuffer =
        IndexBuffer::create(m_indices.data(), m_indices.size(),
                            sizeof(uint32_t), BufferIOType::DYNAMIC);

    m_vertexArray = VertexArray::create();
    VertexBufferLayout layout;
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 2);
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 3);
    m_vertexArray->addVertexBuffer(m_vertexBuffer, layout);
    m_vertexArray->setIndexBuffer(m_indexBuffer);
}

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<uint32_t> &indices)
    : m_vertices(vertices),
      m_indices(indices),
      m_topology(MeshTopology::TRIANGLES),
      m_materialId(0),
      m_wireframe(false) {
    m_vertexBuffer =
        VertexBuffer::create(m_vertices.data(), m_vertices.size(),
                             sizeof(Vertex), BufferIOType::DYNAMIC);

    m_indexBuffer =
        IndexBuffer::create(m_indices.data(), m_indices.size(),
                            sizeof(uint32_t), BufferIOType::DYNAMIC);

    m_vertexArray = VertexArray::create();
    VertexBufferLayout layout;
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 2);
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 3);
    m_vertexArray->addVertexBuffer(m_vertexBuffer, layout);
    m_vertexArray->setIndexBuffer(m_indexBuffer);
}

void Mesh::setVerices(const std::vector<Vertex> &vertices) {
    m_vertices = vertices;
}

void Mesh::setIndices(const std::vector<uint32_t> &indices) {
    m_indices = indices;
}

void Mesh::update() {
    m_indexBuffer->updateData(m_indices.data(), m_indices.size(),
                              sizeof(uint32_t));
    m_vertexBuffer->updateData(m_vertices.data(), m_vertices.size(),
                               sizeof(Vertex));
}

void Mesh::clear() {
    m_vertices.clear();
    m_indices.clear();
}

VertexArray *Mesh::getVertexArray() const { return m_vertexArray.get(); }

void Mesh::setTopology(MeshTopology topology) { m_topology = topology; }

void Mesh::setWireframe(bool wireframe) { m_wireframe = wireframe; }

bool Mesh::isWireframe() const { return m_wireframe; }

const std::vector<Vertex> &Mesh::getVertices() const { return m_vertices; }

std::vector<Vertex> &Mesh::getVertices() { return m_vertices; }

}  // namespace sd
