#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Graphics/Export.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

#include <map>

namespace SD {

using TextureList = std::map<std::string, Ref<Texture>>;
using MeshList = std::vector<Mesh>;

struct ModelNode {
    uint32_t mesh_id;
    uint32_t transform_id;
};

using ModelTree = std::unordered_map<const Material *, std::vector<ModelNode>>;

class SD_GRAPHICS_API Model {
   public:
    Model() = default;

    const std::vector<Material> &GetMaterials() const { return m_materials; };
    const Material *GetMaterials(uint32_t id) const
    {
        return &m_materials[id];
    };

    const ModelTree &GetNodes() const { return m_nodes; }

    void AddModelNode(const Material *mat, const ModelNode &node)
    {
        m_nodes[mat].emplace_back(node);
    }
    void AddModelNode(const Material *mat, ModelNode &&node)
    {
        m_nodes[mat].emplace_back(node);
    }

    void AddTransform(const glm::mat4 &transform)
    {
        m_transforms.emplace_back(transform);
    }
    const glm::mat4 &GetTransform(uint32_t id) const
    {
        return m_transforms.at(id);
    }
    const std::vector<Mesh> &GetMeshes() const { return m_meshes; }

    void AddMaterial(const Material &material);
    void AddMaterial(Material &&material);
    const Material *GetMaterial(uint32_t id) const { return &m_materials[id]; }

    void AddMesh(const Mesh &mesh);
    void AddMesh(Mesh &&mesh);

    const Mesh *GetMesh(size_t id) const { return &m_meshes.at(id); }

    const TextureList &GetTextures() const { return m_textures; }

    void AddTexture(const std::string &path, const Ref<Texture> &texture)
    {
        m_textures.emplace(path, texture);
    }
    Ref<Texture> GetTexture(const std::string &path)
    {
        return m_textures.at(path);
    }
    bool HasTexture(const std::string &path)
    {
        return m_textures.count(path) != 0;
    }

    void SetPath(const std::string &path) { m_path = path; }
    const std::string &GetPath() const { return m_path; }

   private:
    TextureList m_textures;

    std::vector<Mesh> m_meshes;
    std::vector<glm::mat4> m_transforms;
    std::vector<Material> m_materials;

    ModelTree m_nodes;

    std::string m_path;
};

}  // namespace SD

#endif /* SD_MODEL_HPP */
