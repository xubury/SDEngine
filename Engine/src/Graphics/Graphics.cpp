#include "Graphics/Graphics.hpp"
#include "Graphics/ShaderLoader.hpp"
#include "Graphics/TextureLoader.hpp"
#include "Graphics/ModelLoader.hpp"

namespace sd {

static API s_api;
void Graphics::init(API api) {
    SD_CORE_TRACE("Initializing Graphics...");
    Graphics::assetManager().setRootPath("assets/");
    Graphics::assetManager().setLoader<ShaderLoader>();
    Graphics::assetManager().setLoader<TextureLoader>();
    Graphics::assetManager().setLoader<ModelLoader>();

    switch (api) {
        case API::OpenGL:
            Graphics::assetManager()
                .getLoader<TextureLoader>()
                .setFlipVertically(true);
            SD_CORE_INFO("Using OpenGL as backend..");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            exit(-1);
    }
    s_api = api;
}

API Graphics::getAPI() { return s_api; }

AssetManager &Graphics::assetManager() {
    static AssetManager s_instance;
    return s_instance;
}

}  // namespace sd
