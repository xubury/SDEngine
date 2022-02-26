#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Utility/Base.hpp"
#include "Renderer/Export.hpp"
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

const std::string SD_RENDERER_API GetMaterialName(MaterialType type);

class SD_RENDERER_API Material {
   public:
    void SetTexture(MaterialType type, Texture *texture);

    bool HasTexture(MaterialType type) const;

    void RemoveTexture(MaterialType type);

    Texture *GetTexture(MaterialType type) const;

    std::unordered_map<MaterialType, Texture *> &GetTextures() {
        return m_textures;
    }

    const std::unordered_map<MaterialType, Texture *> &GetTextures() const {
        return m_textures;
    };

   private:
    std::unordered_map<MaterialType, Texture *> m_textures;
};

}  // namespace SD

#endif /* SD_MATERIAL_HPP */
