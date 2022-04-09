#ifndef SD_MODEL_NODE_HPP
#define SD_MODEL_NODE_HPP

#include "Utility/Math.hpp"
#include "Graphics/Mesh.hpp"

namespace SD {

class SD_GRAPHICS_API ModelNode {
   public:
    ModelNode(const std::string &name, const Matrix4f &transform)
        : m_name(name), m_local_transform(transform)
    {
    }
    ~ModelNode()
    {
        for (auto &child : m_children) {
            delete child;
        }
    }

    void AddMesh(Mesh *mesh) { m_meshes.push_back(mesh); }
    void AddMaterial(Material *material) { m_materials.push_back(material); }
    void AddChild(ModelNode *child) { m_children.push_back(child); }

    const std::string &GetName() const { return m_name; }
    const Matrix4f &GetTransform() const { return m_local_transform; }

    const std::vector<Mesh *> &GetMeshes() const { return m_meshes; }
    std::vector<Mesh *> &GetMeshes() { return m_meshes; }

    const std::vector<Material *> &GetMaterials() const { return m_materials; }
    std::vector<Material *> &GetMaterials() { return m_materials; }

    std::vector<ModelNode *> &GetChildren() { return m_children; }

   private:
    std::string m_name;
    std::vector<Mesh *> m_meshes;
    std::vector<Material *> m_materials;
    std::vector<ModelNode *> m_children;
    Matrix4f m_local_transform;
};

}  // namespace SD

#endif
