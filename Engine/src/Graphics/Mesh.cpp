#include "Graphics/Mesh.hpp"

namespace sd {

Mesh::Mesh() : m_topology(MeshTopology::TRIANGLES), m_wireframe(false) {}

void Mesh::addVertex(const Vertex &vertex) { m_vertices.emplace_back(vertex); }

void Mesh::addVertex(Vertex &&vertex) {
    m_vertices.emplace_back(std::move(vertex));
}

void Mesh::addIndex(uint32_t index) { m_indices.push_back(index); }

void Mesh::setVerices(const std::vector<Vertex> &vertices) {
    m_vertices = vertices;
}

void Mesh::setIndices(const std::vector<uint32_t> &indices) {
    m_indices = indices;
}

void Mesh::clear() {
    m_vertices.clear();
    m_indices.clear();
}

void Mesh::init() {
    m_vertexArray = VertexArray::create();
    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(3);

    m_vertexBuffer = VertexBuffer::create(m_vertices.data(),
                                          m_vertices.size() * sizeof(Vertex),
                                          BufferIOType::DYNAMIC);

    Ref<IndexBuffer> ebo = IndexBuffer::create(
        m_indices.data(), m_indices.size(), BufferIOType::STATIC);

    m_vertexArray->addVertexBuffer(m_vertexBuffer, layout);
    m_vertexArray->setIndexBuffer(ebo);
}

void Mesh::update() {
    if (m_vertexBuffer) {
        m_vertexBuffer->updateData(m_vertices.data(),
                                   m_vertices.size() * sizeof(Vertex));
    }
}

void Mesh::setTopology(MeshTopology topology) { m_topology = topology; }

void Mesh::setWireframe(bool wireframe) { m_wireframe = wireframe; }

bool Mesh::isWireframe() const { return m_wireframe; }

}  // namespace sd
