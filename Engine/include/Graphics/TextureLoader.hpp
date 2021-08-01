#ifndef SD_TEXTURE_LOADER_HPP
#define SD_TEXTURE_LOADER_HPP

#include "Utils/AssetLoader.hpp"
#include "Graphics/Texture.hpp"

namespace sd {

class TextureLoader : public AssetLoader<Texture> {
   public:
    TextureLoader(AssetManager &manager);

    Ref<Texture> loadAsset(const std::string &path);
};

}  // namespace sd

#endif /* SD_TEXTURE_LOADER_HPP */
