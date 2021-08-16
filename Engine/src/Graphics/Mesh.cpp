#include "Graphics/Mesh.hpp"

namespace sd {

Mesh::Mesh() : m_topology(MeshTopology::TRIANGLES) {}

void Mesh::addVertex(const Vertex &vertex) { m_vertices.emplace_back(vertex); }

void Mesh::addVertex(Vertex &&vertex) {
    m_vertices.emplace_back(std::move(vertex));
}

void Mesh::addIndex(uint32_t index) { m_indices.push_back(index); }

void Mesh::init() {
    m_vertexArray = VertexArray::create();
    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(3);

    Ref<VertexBuffer> vbo = VertexBuffer::create(
        m_vertices.data(), m_vertices.size() * sizeof(Vertex),
        BufferIOType::STATIC);

    Ref<IndexBuffer> ebo = IndexBuffer::create(
        m_indices.data(), m_indices.size(), BufferIOType::STATIC);

    m_vertexArray->addVertexBuffer(vbo, layout);
    m_vertexArray->setIndexBuffer(ebo);
}

void Mesh::setTopology(MeshTopology topology) { m_topology = topology; }

}  // namespace sd
