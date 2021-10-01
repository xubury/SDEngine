#ifndef SD_FONT_LOADER_HPP
#define SD_FONT_LOADER_HPP

#include "Utility/Loader/AssetLoader.hpp"
#include "Graphics/Font.hpp"

namespace sd {

class FontLoader : public AssetLoader<Font> {
   public:
    FontLoader(AssetManager &manager);
    ~FontLoader();

    Ref<Font> loadAsset(const std::string &filePath) override;
};

}  // namespace sd

#endif /* SD_FONT_LOADER_HPP */
