#include "Loader/TextureLoader.hpp"
#include "Loader/ImageLoader.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/String.hpp"
#include "Resource/ResourceRegistry.hpp"

#include <fstream>

namespace SD {

Ref<Texture> TextureLoader::Load(const std::string &path,
                                 const TextureParameter &param,
                                 ResourceRegistry &registry)
{
    auto img = registry.LoadImage(ResourceId(path), path);
    Ref<Texture> texture =
        Texture::Create(img->Width(), img->Height(), 1, MultiSampleLevel::None,
                        TextureType::Normal2D, img->GetDataFormat(), param);
    texture->SetPixels(0, 0, 0, img->Width(), img->Height(), 1, img->Data());
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
