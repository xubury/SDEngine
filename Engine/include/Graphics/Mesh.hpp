#ifndef SD_MESH_HPP
#define SD_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include "Graphics/VertexArray.hpp"

namespace sd {

struct SD_API Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 biTangent;
};

class SD_API Mesh {
   public:
    Mesh();

    void addVertex(const Vertex &vertex);

    void addVertex(Vertex &&vertex);

    void addIndex(uint32_t index);

    void setVerices(const std::vector<Vertex> &vertices);

    void setIndices(const std::vector<uint32_t> &indices);

    void clear();

    void init();

    void update();

    Ref<VertexArray> getVertexArray() const { return m_vertexArray; }

    void setTopology(MeshTopology topology);

    MeshTopology getTopology() const { return m_topology; }

    void setWireframe(bool wireframe);

    bool isWireframe() const;

   private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    Ref<VertexBuffer> m_vertexBuffer;
    Ref<VertexArray> m_vertexArray;
    MeshTopology m_topology;
    bool m_wireframe;
};

}  // namespace sd

#endif /* SD_MESH_HPP */
