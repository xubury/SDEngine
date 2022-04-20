#include "Resource/TextureLoader.hpp"
#include "Graphics/Texture.hpp"
#include "Resource/ImageLoader.hpp"

#include <fstream>

namespace SD {

Ref<Texture> TextureLoader::Load(const std::string &path,
                                 const TextureParameter &param)
{
    ImageLoader loader;
    auto img = loader.Load(path);
    Ref<Texture> texture =
        Texture::Create(img->Width(), img->Height(), 1, MultiSampleLevel::None,
                        TextureType::Normal2D, img->GetDataFormat(), param);
    texture->SetPixels(0, 0, 0, img->Width(), img->Height(), 1, img->Data());
    return texture;
}

Ref<Texture> TextureLoader::Load(const std::array<std::string_view, 6> &pathes)
{
    Ref<Texture> texture;
    TextureParameter param{TextureWrap::Edge, TextureMinFilter::Linear,
                           TextureMagFilter::Linear, MipmapMode::None};
    ImageLoader loader;
    for (int face = 0; face < 6; ++face) {
        auto image = loader.Load(pathes[face]);
        int32_t width = image->Width();
        int32_t height = image->Height();
        if (face == 0) {
            texture = Texture::Create(width, height, 0, MultiSampleLevel::None,
                                      TextureType::Cube, image->GetDataFormat(),
                                      param);
        }
        texture->SetPixels(0, 0, face, width, height, 1, image->Data());
    }
    return texture;
}

// void TextureLoader::WriteToFile(const Texture& texture, const std::string&
// path)
// {
//     std::ofstream os;
//     os.open(path, std::ios::binary);
//     cereal::PortableBinaryOutputArchive archive(os);

//     std::vector<char> data(texture.GetDataSize());
//     texture.ReadPixels(0, 0, 0, 0, texture.GetWidth(), texture.GetHeight(),
//     1,
//                        data.size(), data.data());
//     archive(texture.GetWidth(), texture.GetHeight(), texture.GetFormat(),
//             texture.GetWrap(), texture.GetMinFilter(),
//             texture.GetMagFilter(), texture.GetMipmapMode(),
//             texture.GetMipmapLevels(), data);
// }

}  // namespace SD
