#ifndef SD_MODEL_HPP
#define SD_MODEL_HPP

#include "Graphics/Mesh.hpp"

namespace sd {

class Model {
   public:
    const std::vector<Mesh> &getMeshes() const { return m_meshes; };

    void init();

    void addMesh(const Mesh &mesh);
    void addMesh(Mesh &&mesh);

    void clear();

   private:
    std::vector<Mesh> m_meshes;
};

}  // namespace sd

#endif /* SD_MODEL_HPP */
