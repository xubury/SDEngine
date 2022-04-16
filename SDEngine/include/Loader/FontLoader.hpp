#ifndef SD_FONT_LOADER_HPP
#define SD_FONT_LOADER_HPP

#include "Loader/Export.hpp"
#include "Graphics/Font.hpp"

typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;

namespace SD {

class SD_LOADER_API FontLoader {
   public:
    FontLoader();
    ~FontLoader();
    Ref<Font> Load(const std::string &path, int32_t pixel_height);

   private:
    void LoadGlyph(FT_Face face, Font *font);
    void LoadRangedGlyph(FT_Face face, Font *font, char32_t start,
                         char32_t end);
    FT_Library m_library;
};

}  // namespace SD

#endif /* SD_FONT_LOADER_HPP */
