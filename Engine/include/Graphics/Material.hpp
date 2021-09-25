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
        bool isColor = false;
        std::array<float, 4> color;
        std::string path;
        Ref<Texture> texture;

        template <typename Archive>
        void serialize(Archive &archive) {
            archive(isColor, color, path);
        }

        void init();
    };

   public:
    void init();

    void setTexture(MaterialType type, const TextureProp &prop);

    bool hasTexture(MaterialType type) const;

    void removeTexture(MaterialType type);

    Texture *getTexture(MaterialType type) const;

    std::unordered_map<MaterialType, TextureProp> &getTextures() {
        return m_textures;
    }

    const std::unordered_map<MaterialType, TextureProp> &getTextures() const {
        return m_textures;
    };

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_textures);
    }

   private:
    std::unordered_map<MaterialType, TextureProp> m_textures;
};

}  // namespace sd

#endif /* SD_MATERIAL_HPP */
