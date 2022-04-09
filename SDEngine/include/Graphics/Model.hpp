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
    Material *GetMaterial(int32_t id) { return &m_materials.at(id); }

    void AddMesh(Mesh &&mesh) { m_meshes.push_back(std::move(mesh)); }
    Mesh *GetMesh(int32_t id) { return &m_meshes.at(id); }

    void AddTexture(const std::string &path, const Ref<Texture> &texture)
    {
        m_textures[path] = texture;
    }
    bool HasTexture(const std::string &path) const
    {
        return m_textures.count(path) != 0;
    }
    Texture *GetTexture(const std::string &path)
    {
        return m_textures.at(path).get();
    }

   private:
    ModelNode *m_root_node;
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    std::unordered_map<std::string, Ref<Texture>> m_textures;
};

}  // namespace SD

#endif /* SD_MODEL_HPP */
