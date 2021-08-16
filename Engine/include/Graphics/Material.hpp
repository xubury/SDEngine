#ifndef SD_MATERIAL_HPP
#define SD_MATERIAL_HPP

#include "Common/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Shader.hpp"
#include <string>

namespace sd {

class SD_API Material {
   public:
    struct SD_API TextureProp {
        Ref<Texture> texture;
    };

   public:
    Material() = default;

    void setShader(const Ref<Shader> &shader);

    void setTexture(const std::string &name, const Ref<Texture> &texture);

    void removeTexture(const std::string &name);

    void bind() const;

   private:
    Ref<Shader> m_shader;
    std::unordered_map<std::string, TextureProp> m_textures;
};

}  // namespace sd

#endif /* SD_MATERIAL_HPP */
