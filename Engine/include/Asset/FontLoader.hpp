#ifndef SD_FONT_LOADER_HPP
#define SD_FONT_LOADER_HPP

#include "Asset/Asset.hpp"
#include "Renderer/Font.hpp"

typedef struct FT_LibraryRec_ *FT_Library;

namespace SD {

class SD_API FontLoader : public AssetLoaderBase {
   public:
    FontLoader(AssetManager &manager);

    Ref<void> LoadAsset(const std::filesystem::path &filename) override;

    ~FontLoader();

   private:
    FT_Library m_ft;
};

}  // namespace SD

#endif /* SD_FONT_LOADER_HPP */
