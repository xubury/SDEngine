#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Asset/Export.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

namespace SD {

class SD_ASSET_API Model {
   public:
    Model() = default;

    const std::vector<Mesh> &GetMeshes() const { return m_meshes; };
    std::vector<Mesh> &GetMeshes() { return m_meshes; };

    const std::vector<Material> &GetMaterial() const { return m_materials; };
    std::vector<Material> &GetMaterials() { return m_materials; };

    void AddMesh(const Mesh &mesh);
    void AddMesh(Mesh &&mesh);

    void AddMaterial(const Material &material);
    void AddMaterial(Material &&material);

    void Clear();

   private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
};

}  // namespace SD

#endif /* SD_MODEL_HPP */
