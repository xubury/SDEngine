#ifndef SD_TEXTURE_LOADER_HPP
#define SD_TEXTURE_LOADER_HPP

#include "Utility/Loader/AssetLoader.hpp"
#include "Graphics/Texture.hpp"

namespace sd {

class SD_API TextureLoader : public AssetLoader<Texture> {
   public:
    TextureLoader(AssetManager &manager);
    ~TextureLoader();

    // TODO: perhaps I should make it parse json (compatibility for cubemap)
    Ref<Texture> loadAsset(const std::string &filePath) override;

   private:
};

}  // namespace sd

#endif /* SD_TEXTURE_LOADER_HPP */
