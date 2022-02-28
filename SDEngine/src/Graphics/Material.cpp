#include "Graphics/Material.hpp"
#include <map>

namespace SD {

const std::string GetMaterialName(MaterialType type) {
    static const std::unordered_map<MaterialType, std::string>
        stringByMaterialType = {
            {MaterialType::AMBIENT, "Ambient"},
            {MaterialType::DIFFUSE, "Diffuse"},
            {MaterialType::SPECULAR, "Specular"},
            {MaterialType::EMISSIVE, "Emissive"},
            {MaterialType::HEIGHT, "Height"},
            {MaterialType::NORMALS, "Normals"},
            {MaterialType::SHININESS, "Shininess"},

        };
    return stringByMaterialType.at(type);
}

void Material::SetTexture(MaterialType type, const Ref<Texture> &texture) {
    m_textures[type] = texture;
}

bool Material::HasTexture(MaterialType type) const {
    return m_textures.count(type);
}

void Material::RemoveTexture(MaterialType type) {
    auto iter = m_textures.find(type);
    if (iter != m_textures.end()) {
        m_textures.erase(iter);
    } else {
        SD_CORE_WARN(
            "[Material::removeTexture] Try to remove a non-existent texture!");
    }
}

const Texture *Material::GetTexture(MaterialType type) const {
    return HasTexture(type) ? m_textures.at(type).get() : nullptr;
}

}  // namespace SD
