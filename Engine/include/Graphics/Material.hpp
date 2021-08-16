#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Common/Export.hpp"
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
   public:
    struct SD_API TextureProp {
        Ref<Texture> texture;
    };

   public:
    Material() = default;

    void setShader(const Ref<Shader> &shader);

    void addTexture(MaterialType type, const Ref<Texture> &texture);

    bool hasTexture(MaterialType type) const;

    void removeTexture(MaterialType type);

    Ref<Texture> getTexture(MaterialType type) const;

   private:
    Ref<Shader> m_shader;
    std::unordered_map<MaterialType, TextureProp> m_textures;
};

}  // namespace sd

#endif /* SD_MATERIAL_HPP */
