#include "Graphics/Graphics.hpp"
#include "Graphics/ShaderLoader.hpp"
#include "Graphics/TextureLoader.hpp"

namespace sd {

static API s_api;
void Graphics::init(API api) {
    SD_CORE_TRACE("Initializing Graphics...");
    switch (api) {
        case API::OpenGL:
            SD_CORE_INFO("Using OpenGL as backend..");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            exit(-1);
    }
    s_api = api;
    Graphics::assetManager().setRootPath("assets/");
    Graphics::assetManager().setLoader<ShaderLoader>();
    Graphics::assetManager().setLoader<TextureLoader>();
}

API Graphics::getAPI() { return s_api; }

AssetManager &Graphics::assetManager() {
    static AssetManager s_instance;
    return s_instance;
}

}  // namespace sd
