#include "Graphics/Graphics.hpp"
#include "Utility/Loader/TextureLoader.hpp"

namespace sd {

static API s_api;

void Graphics::setAPI(API api) {
    switch (api) {
        case API::OpenGL:
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
