#include "Loader/TextureLoader.hpp"
#include "Resource/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Bitmap.hpp"
#include "Utility/String.hpp"

#include <fstream>

namespace SD {

Ref<Texture> TextureLoader::LoadTexture2D(const std::string& path)
{
    std::ifstream is(path, std::ios::binary);

    cereal::PortableBinaryInputArchive archive(is);
    ResourceId id;
    TextureWrap wrap;
    TextureMinFilter min_filter;
    TextureMagFilter mag_filter;
    MipmapMode mipmap_mode;
    archive(id, wrap, min_filter, mag_filter, mipmap_mode);

    auto& resource = ResourceManager::Get();
    auto bmp = resource.GetResource<Bitmap>(id);
    int32_t width = bmp->Width();
    int32_t height = bmp->Height();
    Ref<Texture> texture = Texture::Create(
        width, height, 0, MultiSampleLevel::None, TextureType::Normal,
        bmp->GetDataFormat(), wrap, min_filter, mag_filter, mipmap_mode);
    texture->SetPixels(0, 0, 0, width, height, 1, bmp->Data());
    return texture;
}

void TextureLoader::SaveTexture(const Texture& texture, const std::string& path)
{
    std::ofstream os(path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(os);
    ResourceId id(path);
    TextureWrap wrap = texture.GetWrap();
    TextureMinFilter min_filter = texture.GetMinFilter();
    TextureMagFilter mag_filter = texture.GetMagFilter();
    MipmapMode mipmap_mode = texture.GetMipmapMode();
    archive(id, wrap, min_filter, mag_filter, mipmap_mode);
}

}  // namespace SD
