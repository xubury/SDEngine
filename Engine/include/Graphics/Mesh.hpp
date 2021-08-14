#ifndef SD_MESH_HPP
#define SD_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include "Graphics/VertexArray.hpp"

namespace sd {

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 biTangent;
};

class Mesh {
   public:
    Mesh() = default;

    void addVertex(const Vertex &vertex);

    void addVertex(Vertex &&vertex);

    void addIndex(uint32_t index);

    void init();

    Ref<VertexArray> getVertexArray() const { return m_vertexArray; }

   private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    Ref<VertexArray> m_vertexArray;
};

}  // namespace sd

#endif /* SD_MESH_HPP */
