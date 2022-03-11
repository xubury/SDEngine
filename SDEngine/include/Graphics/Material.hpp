#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Shader.hpp"

#include <string>
#include <unordered_map>

namespace SD {

enum class MaterialType {
    NONE,
    DIFFUSE,
    SPECULAR,
    AMBIENT,
    EMISSIVE,
    HEIGHT,
    NORMALS,
    SHININESS
};

const std::string SD_GRAPHICS_API GetMaterialName(MaterialType type);

class SD_GRAPHICS_API Material {
   public:
    void SetTexture(MaterialType type, const Texture *texture);

    bool HasTexture(MaterialType type) const;

    void RemoveTexture(MaterialType type);

    const Texture *GetTexture(MaterialType type) const;

    const std::unordered_map<MaterialType, const Texture *> &GetTextures()
        const {
        return m_textures;
    };

   private:
    std::unordered_map<MaterialType, const Texture *> m_textures;
};

}  // namespace SD

#endif /* SD_MATERIAL_HPP */
