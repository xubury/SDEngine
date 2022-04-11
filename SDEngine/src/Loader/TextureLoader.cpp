#include "Loader/TextureLoader.hpp"
#include "Loader/ImageLoader.hpp"
#include "Resource/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/String.hpp"

#include <fstream>

namespace SD {

Ref<Texture> TextureLoader::LoadFromFile(const std::string& path)
{
    auto bmp = ImageLoader::LoadFromFile(path);
    int32_t width = bmp->Width();
    int32_t height = bmp->Height();
    Ref<Texture> texture = Texture::Create(
        width, height, 0, MultiSampleLevel::None, TextureType::Normal,
        bmp->GetDataFormat(), TextureWrap::Edge, TextureMinFilter::Linear,
        TextureMagFilter::Linear, MipmapMode::Linear);
    texture->SetPixels(0, 0, 0, width, height, 1, bmp->Data());
    return texture;
}

void TextureLoader::SaveTexture(const Texture&, const std::string&) {}

}  // namespace SD
