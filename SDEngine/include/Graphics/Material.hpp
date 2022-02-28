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
    void SetTexture(MaterialType type, const Ref<Texture> &texture);
    void SetPath(MaterialType type, const std::string &path) {
        m_paths[type] = path;
    }

    bool HasTexture(MaterialType type) const;

    void RemoveTexture(MaterialType type);

    const Texture *GetTexture(MaterialType type) const;

    std::string GetPath(MaterialType type) const { return m_paths.at(type); }

    const std::unordered_map<MaterialType, Ref<Texture>> &GetTextures() const {
        return m_textures;
    };

   private:
    std::unordered_map<MaterialType, Ref<Texture>> m_textures;
    std::unordered_map<MaterialType, std::string> m_paths;
};

}  // namespace SD

#endif /* SD_MATERIAL_HPP */
