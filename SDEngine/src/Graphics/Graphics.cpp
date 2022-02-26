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

}  // namespace SD
