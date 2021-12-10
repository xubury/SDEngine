#ifndef SD_FONT_LOADER_HPP
#define SD_FONT_LOADER_HPP

#include "Renderer/Export.hpp"
#include "Asset/Asset.hpp"

typedef struct FT_LibraryRec_ *FT_Library;

namespace SD {

class SD_RENDERER_API FontLoader : public AssetLoader {
   public:
    FontLoader(AssetManager &manager);

    Ref<void> LoadAsset(const std::string &path) override;

    ~FontLoader();

   private:
    FT_Library m_ft;
};

}  // namespace SD

#endif /* SD_FONT_LOADER_HPP */
