#include "Graphics/Material.hpp"
#include "Common/Log.hpp"

namespace sd {

void Material::setShader(const Ref<Shader> &shader) { m_shader = shader; }

void Material::addTexture(MaterialType type, const Ref<Texture> &texture) {
    if (texture) {
        TextureProp prop;
        prop.texture = texture;
        m_textures[type] = prop;
    }
}

bool Material::hasTexture(MaterialType type) const {
    return m_textures.count(type);
}

void Material::removeTexture(MaterialType type) {
    auto iter = m_textures.find(type);
    if (iter != m_textures.end()) {
        m_textures.erase(iter);
    } else {
        SD_CORE_WARN(
            "[Material::removeTexture] Try to remove a non-existent texture!");
    }
}

Ref<Texture> Material::getTexture(MaterialType type) const {
    return hasTexture(type) ? m_textures.at(type).texture : nullptr;
}

}  // namespace sd
