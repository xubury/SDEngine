#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

namespace sd {

class SD_API Model {
   public:
    const std::vector<Mesh> &getMeshes() const { return m_meshes; };
    const std::vector<Material> &getMaterials() const { return m_materials; }

    void init();

    void addMesh(const Mesh &mesh);
    void addMesh(Mesh &&mesh);

    void addMaterial(const Material &mesh);
    void addMaterial(Material &&mesh);

    void clear();

    Ref<Model> clone();
   private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
};

}  // namespace sd

#endif /* SD_MODEL_HPP */
