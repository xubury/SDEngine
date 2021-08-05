#include "Graphics/Graphics.hpp"
namespace sd {

AssetManager &Graphics::assetManager() {
    static AssetManager s_instance;
    return s_instance;
}

}  // namespace sd
