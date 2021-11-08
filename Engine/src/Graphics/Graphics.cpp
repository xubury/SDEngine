#include "Graphics/Graphics.hpp"
#include "Utility/Log.hpp"

namespace sd {

static GraphicsAPI s_api;

void setGraphicsAPI(GraphicsAPI api) {
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

GraphicsAPI getGraphicsAPI() { return s_api; }

}  // namespace sd
