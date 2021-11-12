#include "Renderer/Material.hpp"
#include <map>

namespace SD {

const std::unordered_map<MaterialType, std::string> stringByMaterialType = {
    {MaterialType::AMBIENT, "Ambient"},
    {MaterialType::DIFFUSE, "Diffuse"},
    {MaterialType::SPECULAR, "Specular"},
    {MaterialType::EMISSIVE, "Emissive"},
    {MaterialType::HEIGHT, "Height"},
    {MaterialType::NORMALS, "Normals"},
    {MaterialType::SHININESS, "Shininess"},

};

std::string getMaterialName(MaterialType type) {
    return stringByMaterialType.at(type);
}

void Material::setTexture(MaterialType type, const Ref<Texture> &texture) {
    m_textures[type] = texture;
}

bool Material::hasTexture(MaterialType type) const {
    return m_textures.count(type);
}

void Material::removeTexture(MaterialType type) {
    auto iter = m_textures.find(type);
    if (iter != m_textures.end()) {
        m_textures.erase(iter);
    } else {
        SD_CORE_WARN(
            "[Material::removeTexture] Try to remove a non-existent texture!");
    }
}

Texture *Material::getTexture(MaterialType type) const {
    return hasTexture(type) ? m_textures.at(type).get() : nullptr;
}

}  // namespace SD
