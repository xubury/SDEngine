#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Utility/Base.hpp"
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

std::string SD_API GetMaterialName(MaterialType type);

class SD_API Material {
   public:
    void SetTexture(MaterialType type, const Ref<Texture> &texture);

    bool HasTexture(MaterialType type) const;

    void RemoveTexture(MaterialType type);

    Texture *GetTexture(MaterialType type) const;

    std::unordered_map<MaterialType, Ref<Texture>> &GetTextures() {
        return m_textures;
    }

    const std::unordered_map<MaterialType, Ref<Texture>> &GetTextures() const {
        return m_textures;
    };

   private:
    std::unordered_map<MaterialType, Ref<Texture>> m_textures;
};

}  // namespace SD

#endif /* SD_MATERIAL_HPP */
