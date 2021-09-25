#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Utils/Export.hpp"
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
    struct SD_API TextureProp {
        Ref<Texture> texture;
        std::string path;
    };

   public:
    Material();

    void setTexture(MaterialType type, const TextureProp &prop);

    bool hasTexture(MaterialType type) const;

    void removeTexture(MaterialType type);

    Texture *getTexture(MaterialType type) const;
   private:
    std::unordered_map<MaterialType, TextureProp> m_textures;
};

}  // namespace sd

#endif /* SD_MATERIAL_HPP */
