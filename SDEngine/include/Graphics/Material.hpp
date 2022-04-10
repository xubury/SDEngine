#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/Math.hpp"
#include <string>
#include <unordered_map>

namespace SD {

enum class MaterialType {
    None,
    Diffuse,
    Specular,
    Ambient,
    Emissive,
    Normal,
    Shininess
};

const std::string SD_GRAPHICS_API GetMaterialName(MaterialType type);

class SD_GRAPHICS_API Material {
   public:
    Material();
    void SetResource(MaterialType type, ResourceId rid);
    const Texture *GetTexture(MaterialType type) const;

    void SetAmbientColor(const Vector3f &color) { m_ambient_base = color; }
    void SetDiffuseColor(const Vector3f &color) { m_diffuse_base = color; }
    void SetEmissiveColor(const Vector3f &color) { m_emissive_base = color; }
    const Vector3f &GetAmbientColor() const { return m_ambient_base; }
    const Vector3f &GetDiffuseColor() const { return m_diffuse_base; }
    const Vector3f &GetEmissiveColor() const { return m_emissive_base; }

    SERIALIZE(m_textures, m_diffuse_base, m_ambient_base, m_emissive_base)
   private:
    std::unordered_map<MaterialType, ResourceId> m_textures;
    Vector3f m_diffuse_base;
    Vector3f m_ambient_base;
    Vector3f m_emissive_base;
};

}  // namespace SD

#endif /* SD_MATERIAL_HPP */
