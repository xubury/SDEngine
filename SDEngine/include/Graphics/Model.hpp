#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Graphics/Export.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/ModelNode.hpp"

namespace SD {

class SD_GRAPHICS_API Model {
   public:
    Model() = default;
    virtual ~Model() { delete m_root_node; };

    void SetRootNode(ModelNode *node) { m_root_node = node; }
    ModelNode *GetRootNode() const { return m_root_node; }

    void AddMaterial(Material &&material)
    {
        m_materials.push_back(std::move(material));
    }
    const Material &GetMaterial(int32_t id) const { return m_materials.at(id); }

    void AddMesh(Mesh &&mesh) { m_meshes.push_back(std::move(mesh)); }
    const Mesh &GetMesh(int32_t id) const { return m_meshes.at(id); }

   private:
    ModelNode *m_root_node;
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
};

}  // namespace SD

#endif /* SD_MODEL_HPP */
