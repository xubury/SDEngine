#include "Loader/ImageLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace SD {

// FIXME: 16bit should use this stbi_load_16();
Ref<ByteImage> ImageLoader::Load(const std::string& path)
{
    SD_CORE_TRACE("Loading image from: {}", path);
    int32_t width;
    int32_t height;
    int32_t channels;
    uint8_t* img = stbi_load(path.c_str(), &width, &height, &channels, 0);
    auto bmp = CreateRef<ByteImage>(width, height, channels, img);
    stbi_image_free(img);
    return bmp;
}

Ref<ByteImage> ImageLoader::Load(const uint8_t* data, int32_t size)
{
    int32_t width;
    int32_t height;
    int32_t channels;
    uint8_t* img =
        stbi_load_from_memory(data, size, &width, &height, &channels, 0);
    auto byte_img = CreateRef<ByteImage>(width, height, channels, img);
    stbi_image_free(img);
    return byte_img;
}

}  // namespace SD
