#include "Graphics/Material.hpp"
#include "Utils/Log.hpp"

namespace sd {

void Material::TextureProp::init() {
    if (isColor) {
        texture = Texture::create(1, 1, 1, TextureType::TEX_2D,
                                  TextureFormat::RGBA, TextureFormatType::FLOAT,
                                  TextureWrap::REPEAT, TextureFilter::LINEAR,
                                  TextureMipmapFilter::LINEAR, color.data());
    } else {
        texture = Graphics::assetManager().load<Texture>(path);
    }
}

void Material::init() {
    for (auto &[type, texture] : getTextures()) {
        texture.init();
    }
}

void Material::setTexture(MaterialType type, const TextureProp &prop) {
    m_textures[type] = prop;
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

Texture *Material::getTexture(MaterialType type) const {
    return hasTexture(type) ? m_textures.at(type).texture.get() : nullptr;
}

}  // namespace sd
