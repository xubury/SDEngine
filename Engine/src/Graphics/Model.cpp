#include "Graphics/Model.hpp"

namespace sd {

void Model::init() {
    for (auto &mesh : m_meshes) {
        mesh.init();
    }
}

void Model::addMesh(const Mesh &mesh) { m_meshes.emplace_back(mesh); }

void Model::addMesh(Mesh &&mesh) { m_meshes.emplace_back(std::move(mesh)); }

void Model::clear() { m_meshes.clear(); }

}  // namespace sd
