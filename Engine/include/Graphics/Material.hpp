#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Utility/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Shader.hpp"
#include <string>

namespace sd {

enum class MaterialType {
    DIFFUSE,
    SPECULAR,
    AMBIENT,
    EMISSIVE,
    HEIGHT,
    NORMALS,
    SHININESS
};

class SD_API Material {
   public:
    void init();

    void setTexture(MaterialType type, const Ref<Texture> &texture);

    bool hasTexture(MaterialType type) const;

    void removeTexture(MaterialType type);

    Texture *getTexture(MaterialType type) const;

    std::unordered_map<MaterialType, Ref<Texture>> &getTextures() {
        return m_textures;
    }

    const std::unordered_map<MaterialType, Ref<Texture>> &getTextures() const {
        return m_textures;
    };

   private:
    std::unordered_map<MaterialType, Ref<Texture>> m_textures;
};

}  // namespace sd

#endif /* SD_MATERIAL_HPP */
