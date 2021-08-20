#include "Graphics/Model.hpp"

namespace sd {

void Model::init() {
    for (auto &mesh : m_meshes) {
        mesh.init();
    }
}

void Model::addMesh(const Mesh &mesh) { m_meshes.emplace_back(mesh); }

void Model::addMesh(Mesh &&mesh) { m_meshes.emplace_back(std::move(mesh)); }

void Model::addMaterial(const Material &material) {
    m_materials.emplace_back(material);
}

void Model::addMaterial(Material &&material) {
    m_materials.emplace_back(std::move(material));
}

void Model::clear() { m_meshes.clear(); }

Ref<Model> Model::clone() { return std::make_shared<Model>(*this); }

}  // namespace sd
