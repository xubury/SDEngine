#include "Graphics/Graphics.hpp"

namespace SD {

static GraphicsAPI s_api = GraphicsAPI::None;

void SetGraphicsAPI(GraphicsAPI api) {
    if (s_api != GraphicsAPI::None) {
        SD_CORE_ERROR("Graphics API already set!");
        exit(-1);
    }
    switch (api) {
        case GraphicsAPI::OpenGL:
            SD_CORE_INFO("Using OpenGL as backend..");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            exit(-1);
    }
    s_api = api;
}

GraphicsAPI GetGraphicsAPI() { return s_api; }

DataFormat GetDataFormat(int channels) {
    DataFormat format;
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

DataFormatType GetDataFormatType(int bits_per_pixel, int channels) {
    DataFormatType format_type;
    int bits_per_channel = bits_per_pixel / channels;
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
