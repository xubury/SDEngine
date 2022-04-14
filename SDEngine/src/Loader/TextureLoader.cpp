#include "Loader/TextureLoader.hpp"
#include "Loader/ImageLoader.hpp"
#include "Resource/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/String.hpp"

#include <fstream>

namespace SD {

Ref<Texture> TextureLoader::LoadFromFile(const std::string& path)
{
    std::ifstream is;
    is.open(path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(is);
    int32_t width;
    int32_t height;
    DataFormat format;
    TextureWrap wrap;
    TextureMinFilter min_filter;
    TextureMagFilter mag_filter;
    MipmapMode mipmap;
    int mipmap_levels;
    std::vector<char> data;
    archive(width, height, format, wrap, min_filter, mag_filter, mipmap,
            mipmap_levels, data);

    Ref<Texture> texture = Texture::Create(
        width, height, 0, MultiSampleLevel::None, TextureType::Normal, format,
        wrap, min_filter, mag_filter, mipmap, mipmap_levels);
    texture->SetPixels(0, 0, 0, width, height, 1, data.data());
    return texture;
}

void TextureLoader::WriteToFile(const Texture& texture, const std::string& path)
{
    std::ofstream os;
    os.open(path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(os);

    std::vector<char> data(texture.GetDataSize());
    texture.ReadPixels(0, 0, 0, 0, texture.GetWidth(), texture.GetHeight(), 1,
                       data.size(), data.data());
    archive(texture.GetWidth(), texture.GetHeight(), texture.GetFormat(),
            texture.GetWrap(), texture.GetMinFilter(), texture.GetMagFilter(),
            texture.GetMipmapMode(), texture.GetMipmapLevels(), data);
}

}  // namespace SD
