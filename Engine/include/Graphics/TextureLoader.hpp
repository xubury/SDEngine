#ifndef SD_TEXTURE_LOADER_HPP
#define SD_TEXTURE_LOADER_HPP

#include "Utils/AssetLoader.hpp"
#include "Graphics/Texture.hpp"

namespace sd {

class TextureLoader : public AssetLoader<Texture> {
   public:
    TextureLoader(AssetManager &manager);

    // TODO: perhaps I should make it parse json (compatibility for cubemap)
    Ref<Texture> loadAsset(const std::string &filePath) override;

    void setFlipVertically(bool enable);

   private:
    bool m_flip;
};

}  // namespace sd

#endif /* SD_TEXTURE_LOADER_HPP */
