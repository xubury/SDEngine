#ifndef SD_MODEL_NODE_HPP
#define SD_MODEL_NODE_HPP

#include "Utility/Math.hpp"
#include "Graphics/Mesh.hpp"

namespace SD {

class SD_GRAPHICS_API MeshNode {
   public:
    MeshNode(Mesh *mesh, Material *material)
        : m_mesh(mesh), m_material(material)
    {
    }
    ~MeshNode() = default;

    const Mesh &GetMesh() const { return *m_mesh; }
    const Material &GetMaterial() const { return *m_material; }

    Mesh &GetMesh() { return *m_mesh; }
    Material &GetMaterial() { return *m_material; }

   private:
    Mesh *m_mesh;
    Material *m_material;
};

class SD_GRAPHICS_API ModelNode {
   public:
    ModelNode(const Matrix4f &transform) : m_local_transform(transform) {}

    void AddNode(MeshNode &&node) { m_nodes.push_back(std::move(node)); }

    const Matrix4f &GetTransform() const { return m_local_transform; }

    const std::vector<MeshNode> &GetMeshNodes() const { return m_nodes; }
    std::vector<MeshNode> &GetMeshNodes() { return m_nodes; }

   private:
    std::vector<MeshNode> m_nodes;
    Matrix4f m_local_transform;
};

}  // namespace SD

#endif
