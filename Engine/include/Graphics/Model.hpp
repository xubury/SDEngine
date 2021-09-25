#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

namespace sd {

class SD_API Model {
   public:
    const std::vector<Mesh> &getMeshes() const { return m_meshes; };
    std::vector<Mesh> &getMeshes() { return m_meshes; };

    const Material &getMaterial() const { return m_material; }
    Material &getMaterial() { return m_material; }

    void init();

    void addMesh(const Mesh &mesh);
    void addMesh(Mesh &&mesh);

    void clear();

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_material);
    }

   private:
    std::vector<Mesh> m_meshes;
    Material m_material;
};

}  // namespace sd

#endif /* SD_MODEL_HPP */
