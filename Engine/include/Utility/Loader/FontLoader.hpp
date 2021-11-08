#ifndef SD_FONT_LOADER_HPP
#define SD_FONT_LOADER_HPP

#include "Utility/Loader/AssetLoader.hpp"
#include "Renderer/Font.hpp"

typedef struct FT_LibraryRec_ *FT_Library;

namespace sd {

class FontLoader : public AssetLoader<Font> {
   public:
    FontLoader(AssetManager &manager);
    ~FontLoader();

    Ref<Font> loadAsset(const std::string &filePath) override;

   private:
    FT_Library m_ft;
};

}  // namespace sd

#endif /* SD_FONT_LOADER_HPP */
