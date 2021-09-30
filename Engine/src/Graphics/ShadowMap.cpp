#include "Graphics/ShadowMap.hpp"
#include "Graphics/Texture.hpp"

namespace sd {

ShadowMap::ShadowMap() {
    const uint32_t width = 4096;
    const uint32_t height = 4096;

    m_target.addTexture(Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::UBYTE, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    auto shadowMap = Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::DEPTH,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST);
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    shadowMap->setBorderColor(&color);
    m_target.addTexture(shadowMap);

    m_target.init();
}

void ShadowMap::setTransfrom(Transform *transform) { m_transform = transform; }

}  // namespace sd
