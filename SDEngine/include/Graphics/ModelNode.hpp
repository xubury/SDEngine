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

    void AddMesh(int32_t index) { m_meshes.push_back(index); }
    void AddMaterial(int32_t index) { m_materials.push_back(index); }
    void AddChild(ModelNode *child) { m_children.push_back(child); }

    const std::string &GetName() const { return m_name; }
    const Matrix4f &GetTransform() const { return m_local_transform; }

    const std::vector<uint32_t> &GetMeshes() const { return m_meshes; }
    std::vector<uint32_t> &GetMeshes() { return m_meshes; }

    const std::vector<uint32_t> &GetMaterials() const { return m_materials; }
    std::vector<uint32_t> &GetMaterials() { return m_materials; }

    std::vector<ModelNode *> &GetChildren() { return m_children; }
    const std::vector<ModelNode *> &GetChildren() const { return m_children; }

   private:
    std::string m_name;
    std::vector<uint32_t> m_meshes;
    std::vector<uint32_t> m_materials;
    std::vector<ModelNode *> m_children;
    Matrix4f m_local_transform;
};

}  // namespace SD

#endif
