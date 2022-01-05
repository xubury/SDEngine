#ifndef SD_FONT_LOADER_HPP
#define SD_FONT_LOADER_HPP

#include "Renderer/Export.hpp"
#include "Renderer/Font.hpp"
#include "Asset/AssetManager.hpp"

typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;

namespace SD {

class SD_RENDERER_API FontLoader : public AssetLoader {
   public:
    FontLoader(AssetManager &manager);

    Ref<void> LoadAsset(const std::string &path) override;

    ~FontLoader();

   private:
    void LoadGlyph(Font &font);
    void LoadRangedGlyph(Font &font, char32_t start, char32_t end);

    FT_Library m_ft;
    FT_Face m_face;
};

}  // namespace SD

#endif /* SD_FONT_LOADER_HPP */
