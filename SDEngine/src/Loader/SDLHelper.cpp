#include "Loader/SDLHelper.hpp"
#include "Utility/Base.hpp"

namespace SD {

SDL_Surface* LoadImage(const std::string& path) {
    if (path.empty()) {
        throw Exception("Empty path!");
    }
    SDL_Surface* loaded = IMG_Load(path.c_str());
    SD_CORE_TRACE("Loading image form: {}...", path);
    if (loaded == nullptr) {
        throw Exception(
            fmt::format("Unable to load image {}! SDL_image Error: {}", path,
                        IMG_GetError()));
    }
    return loaded;
}

int GetBitsPerPixel(const SDL_Surface* surface) {
    return surface->format->BitsPerPixel;
}

int GetNumOfChannels(const SDL_Surface* surface) {
    return surface->pitch / surface->w;
}

DataFormat GetDataFormat(const SDL_Surface* surface) {
    DataFormat format;
    const int channels = surface->pitch / surface->w;
    switch (channels) {
        case 1:
            format = DataFormat::RED;
            break;
        case 2:
            format = DataFormat::RG;
            break;
        case 3:
            format = DataFormat::RGB;
            break;
        case 4:
            format = DataFormat::RGBA;
            break;
        default:
            format = DataFormat::RGB;
            SD_CORE_ERROR("[GetDataFormat] Unsupported channels!");
            break;
    }
    return format;
}

DataFormatType GetDataFormatType(const SDL_Surface* surface) {
    DataFormatType format_type;
    const int channels = surface->pitch / surface->w;
    const int bits_per_channel = surface->format->BitsPerPixel / channels;
    switch (bits_per_channel) {
        case 8:
            format_type = DataFormatType::UBYTE;
            break;
        case 32:
            format_type = DataFormatType::UINT;
            break;
        default:
            format_type = DataFormatType::UBYTE;
            SD_CORE_ERROR(
                "[GetDataFormatType] Unsupported bits per "
                "channel!");
    }
    return format_type;
}

}  // namespace SD
