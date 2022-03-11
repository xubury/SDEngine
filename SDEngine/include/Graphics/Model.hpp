#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Graphics/Export.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

#include <map>

namespace SD {

using ImportedTexture = std::map<std::string, Ref<Texture>>;

class SD_GRAPHICS_API Model {
   public:
    Model() = default;

    const std::vector<Mesh> &GetMeshes() const { return m_meshes; };

    const std::vector<Material> &GetMaterials() const { return m_materials; };

    void AddMesh(const Mesh &mesh);
    void AddMesh(Mesh &&mesh);

    void AddMaterial(const Material &material);
    void AddMaterial(Material &&material);

    void Clear();

    void SetPath(const std::string &path) { m_path = path; }
    const std::string &GetPath() const { return m_path; }

    const ImportedTexture &GetImportedTexture() const { return m_imported; }
    void SetImportedTexture(const ImportedTexture &imported) {
        m_imported = imported;
    }

   private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    ImportedTexture m_imported;
    std::string m_path;
};

}  // namespace SD

#endif /* SD_MODEL_HPP */
