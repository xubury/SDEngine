#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

namespace sd {

class SD_API Model {
   public:
    Model() = default;

    const std::vector<Mesh> &getMeshes() const { return m_meshes; };
    std::vector<Mesh> &getMeshes() { return m_meshes; };

    const std::vector<Material> &getMaterial() const { return m_materials; };
    std::vector<Material> &getMaterials() { return m_materials; };

    void addMesh(const Mesh &mesh);
    void addMesh(Mesh &&mesh);

    void addMaterial(const Material &material);
    void addMaterial(Material &&material);

    void clear();

   private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
};

}  // namespace sd

#endif /* SD_MODEL_HPP */
