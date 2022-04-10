#include "Graphics/Material.hpp"
#include "Resource/ResourceManager.hpp"
#include <map>

namespace SD {

const std::string GetMaterialName(MaterialType type)
{
    static const std::unordered_map<MaterialType, std::string>
        stringByMaterialType = {
            {MaterialType::Ambient, "Ambient"},
            {MaterialType::Diffuse, "Diffuse"},
            {MaterialType::Specular, "Specular"},
            {MaterialType::Emissive, "Emissive"},
            {MaterialType::Normal, "Normal"},
            {MaterialType::Shininess, "Shininess"},

        };
    return stringByMaterialType.at(type);
}

Material::Material()
    : m_diffuse_base(1.0), m_ambient_base(1.0), m_emissive_base(0)
{
}

void Material::SetResource(MaterialType type, ResourceId rid)
{
    m_textures[type] = rid;
}

const Texture* Material::GetTexture(MaterialType type) const
{
    auto& resource = ResourceManager::Get();
    if (m_textures.count(type) == 0) {
        return nullptr;
    }
    auto rid = m_textures.at(type);
    return resource.Exist<Texture>(rid)
               ? resource.GetResource<Texture>(rid).get()
               : nullptr;
}

}  // namespace SD
