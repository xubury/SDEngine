#include "Loader/MaterialLoader.hpp"

namespace SD {

Ref<Material> MaterialLoader::Load(const std::string &path)
{
    Vector3f diffuse_base;
    Vector3f ambient_base;
    Vector3f emissive_base;
}

void MaterialLoader::Save(const Material &material, const std::string &path) {}

}  // namespace SD
