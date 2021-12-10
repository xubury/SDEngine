#include "Renderer/Model.hpp"

namespace SD {

void Model::AddMesh(const Mesh &mesh) { m_meshes.emplace_back(mesh); }

void Model::AddMesh(Mesh &&mesh) { m_meshes.emplace_back(std::move(mesh)); }

void Model::AddMaterial(const Material &material) {
    m_materials.emplace_back(material);
}

void Model::AddMaterial(Material &&material) {
    m_materials.emplace_back(std::move(material));
}

void Model::Clear() { m_meshes.clear(); }

}  // namespace SD
