#include "Graphics/Graphics.hpp"
#include "Graphics/TextureLoader.hpp"

namespace sd {

static API s_api;

void Graphics::init(API api) {
    SD_CORE_TRACE("Initializing Graphics...");

    switch (api) {
        case API::OpenGL:
            Asset::manager().getLoader<TextureLoader>().setFlipVertically(true);
            SD_CORE_INFO("Using OpenGL as backend..");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            exit(-1);
    }
    s_api = api;
}

API Graphics::getAPI() { return s_api; }

}  // namespace sd
