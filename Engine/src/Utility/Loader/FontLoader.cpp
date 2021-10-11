#include "Utility/Loader/FontLoader.hpp"
#include "Utility/SDLHelper.hpp"
#include "Graphics/Font.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace sd {

FontLoader::FontLoader(AssetManager &manager) : AssetLoader<Font>(manager) {}

Ref<Font> FontLoader::loadAsset(const std::string &filePath) {
    Ref<Font> font;
    do {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            SD_CORE_ERROR("Could not init Freetype library!");
            break;
        }

        FT_Face face;
        if (FT_New_Face(ft, filePath.c_str(), 0, &face)) {
            SD_CORE_ERROR("Failed to load font!");
            break;
        }
        int pixelSize = 18;
        FT_Set_Pixel_Sizes(face, 0, pixelSize);
        font = createRef<Font>();
        font->setPixelSize(pixelSize);
        Character c;
        for (wchar_t ch = 0; ch < 128; ++ch) {
            if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
                SD_CORE_WARN("Failed to load glyph!");
                continue;
            }
            c.size =
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            c.bearing =
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            c.advance = face->glyph->advance.x;
            c.texture = Texture::create(
                face->glyph->bitmap.width, face->glyph->bitmap.rows, 1,
                TextureType::TEX_2D, TextureFormat::ALPHA,
                TextureFormatType::UBYTE, TextureWrap::EDGE,
                TextureFilter::LINEAR, TextureMipmapFilter::LINEAR,
                face->glyph->bitmap.buffer);
            font->setCharacter(ch, c);
        }
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    } while (false);

    return font;
}

}  // namespace sd
