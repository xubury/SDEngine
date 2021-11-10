#ifndef SD_FONT_LOADER_HPP
#define SD_FONT_LOADER_HPP

#include "Asset/Asset.hpp"
#include "Renderer/Font.hpp"

typedef struct FT_LibraryRec_ *FT_Library;

namespace sd {

class FontLoader : public AssetLoader<Font> {
   public:
    Ref<void> loadAsset(const std::string &filePath) override;

    ~FontLoader();

   private:
    friend class AssetManager;

    FontLoader();

    FT_Library m_ft;
};

}  // namespace sd

#endif /* SD_FONT_LOADER_HPP */
