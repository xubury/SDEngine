#include "Graphics/Graphics.hpp"
#include "Graphics/ShaderLoader.hpp"
#include "Graphics/TextureLoader.hpp"

namespace sd {

void Graphics::init() {
    SD_CORE_TRACE("Initializing Graphics...");
    Graphics::assetManager().setRootPath("assets/");
    Graphics::assetManager().setLoader<ShaderLoader>();
    Graphics::assetManager().setLoader<TextureLoader>();
}

AssetManager &Graphics::assetManager() {
    static AssetManager s_instance;
    return s_instance;
}

}  // namespace sd
