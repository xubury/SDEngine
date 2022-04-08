#ifndef SD_MODEL_NODE_HPP
#define SD_MODEL_NODE_HPP

#include "Utility/Math.hpp"
#include "Graphics/Mesh.hpp"

namespace SD {

class SD_GRAPHICS_API ModelNode {
   public:
    using MeshMap = std::unordered_map<Material *, std::vector<Mesh *>>;
    ModelNode(const Matrix4f &transform);
    ~ModelNode();

    void AddMesh(Material *material, Mesh *mesh)
    {
        m_meshes[material].push_back(mesh);
    }

    void AddChild(ModelNode *node) { m_children.push_back(node); }

    const Matrix4f &GetLocalTransform() const { return m_local_matrix; }

    const MeshMap &GetMeshes() const { return m_meshes; }

   private:
    Matrix4f m_local_matrix;
    std::vector<ModelNode *> m_children;
    MeshMap m_meshes;
};

}  // namespace SD

#endif
