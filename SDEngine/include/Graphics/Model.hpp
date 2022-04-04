#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Graphics/Export.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

#include <map>

namespace SD {

struct MeshReference {
    uint32_t mesh_id;
    uint32_t transform_id;
};

class SD_GRAPHICS_API Model {
   public:
    using MaterialMap =
        std::unordered_map<const Material *, std::vector<MeshReference>>;
    using TextureList = std::unordered_map<std::string, Ref<Texture>>;
    using MeshList = std::vector<Mesh>;
    using MaterialList = std::vector<Material>;
    using TransformList = std::vector<Matrix4f>;

    Model() = default;

    MaterialList &GetMaterials() { return m_materials; };
    const MaterialList &GetMaterials() const { return m_materials; };
    const Material *GetMaterials(uint32_t id) const
    {
        return &m_materials[id];
    };

    const MaterialMap &GetMaterialMap() const { return m_refs; }

    void MapMaterial(const Material *mat, const MeshReference &node)
    {
        m_refs[mat].emplace_back(node);
    }
    void MapMaterial(const Material *mat, MeshReference &&node)
    {
        m_refs[mat].emplace_back(node);
    }

    void AddTransform(const Matrix4f &transform)
    {
        m_transforms.emplace_back(transform);
    }
    const Matrix4f &GetTransform(uint32_t id) const
    {
        return m_transforms.at(id);
    }
    const MeshList &GetMeshes() const { return m_meshes; }

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
    Texture *GetTexture(const std::string &path)
    {
        return m_textures.at(path).get();
    }
    bool HasTexture(const std::string &path)
    {
        return m_textures.count(path) != 0;
    }

    void SetPath(const std::string &path) { m_path = path; }
    const std::string &GetPath() const { return m_path; }

   private:
    TextureList m_textures;

    MeshList m_meshes;
    TransformList m_transforms;
    MaterialList m_materials;

    MaterialMap m_refs;

    std::string m_path;
};

}  // namespace SD

#endif /* SD_MODEL_HPP */
