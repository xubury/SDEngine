#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Utility/Base.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Shader.hpp"

#include <string>
#include <unordered_map>

namespace SD {

enum class MaterialType {
    DIFFUSE,
    SPECULAR,
    AMBIENT,
    EMISSIVE,
    HEIGHT,
    NORMALS,
    SHININESS
};

std::string SD_API getMaterialName(MaterialType type);

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

}  // namespace SD

#endif /* SD_MATERIAL_HPP */
