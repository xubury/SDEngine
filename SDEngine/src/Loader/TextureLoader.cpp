#include "Loader/TextureLoader.hpp"
#include "Graphics/Texture.hpp"
#include "Utility/String.hpp"

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace SD {

static DataFormat Get8BitDataFormat(int32_t channels)
{
    DataFormat format = DataFormat::RGB8;
    switch (channels) {
        case 1:
            format = DataFormat::R8;
            break;
        case 2:
            format = DataFormat::RG8;
            break;
        case 3:
            format = DataFormat::RGB8;
            break;
        case 4:
            format = DataFormat::RGBA8;
            break;
    }
    return format;
}

Ref<Texture> TextureLoader::LoadTextureCube(
    const std::vector<std::string>& pathes)
{
    Ref<Texture> texture;
    if (pathes.size() < 6) {
        throw Exception("Not enough images to load a texture cube!");
    }
    for (int face = 0; face < 6; ++face) {
        int32_t width;
        int32_t height;
        int32_t channels;
        SD_CORE_TRACE("Loading image from {}", pathes[face]);
        uint8_t* img =
            stbi_load(pathes[face].c_str(), &width, &height, &channels, 0);
        if (face == 0) {
            texture =
                Texture::Create(width, height, 0, MultiSampleLevel::None,
                                TextureType::Cube, Get8BitDataFormat(channels),
                                TextureWrap::Edge, TextureMinFilter::Linear,
                                MipmapMode::Linear, TextureMagFilter::Linear);
        }
        texture->SetPixels(0, 0, face, width, height, 1, img);
        stbi_image_free(img);
    }
    return texture;
}

// FIXME: 16bit should use this stbi_load_16();

Ref<Texture> TextureLoader::LoadTexture2D(const std::string& path)
{
    SD_CORE_TRACE("Loading image from {}", path);
    int32_t width;
    int32_t height;
    int32_t channels;
    uint8_t* img = stbi_load(path.c_str(), &width, &height, &channels, 0);
    Ref<Texture> texture = Texture::Create(
        width, height, 0, MultiSampleLevel::None, TextureType::Normal,
        Get8BitDataFormat(channels), TextureWrap::Edge,
        TextureMinFilter::Linear, MipmapMode::Linear, TextureMagFilter::Linear);
    texture->SetPixels(0, 0, 0, width, height, 1, img);
    stbi_image_free(img);
    return texture;
}

}  // namespace SD
