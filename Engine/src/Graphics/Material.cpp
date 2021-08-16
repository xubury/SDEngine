#include "Graphics/Material.hpp"
#include "Common/Log.hpp"

namespace sd {

void Material::setShader(const Ref<Shader> &shader) { m_shader = shader; }

void Material::setTexture(const std::string &name,
                          const Ref<Texture> &texture) {
    TextureProp prop;
    prop.texture = texture;
    m_textures[name] = prop;
}

void Material::removeTexture(const std::string &name) {
    auto iter = m_textures.find(name);
    if (iter != m_textures.end()) {
        m_textures.erase(iter);
    } else {
        SD_CORE_WARN(
            "[Material::removeTexture] Try to remove a non-existent texture!");
    }
}

void Material::bind() const {
    m_shader->bind();
    for (const auto &[name, texture] : m_textures) {
        m_shader->setInt(name, 0);
        m_shader->setTexture(texture.texture.get(), 0);
    }
}

}  // namespace sd
