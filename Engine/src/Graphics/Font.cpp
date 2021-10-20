#include "Graphics/Font.hpp"
#include "Utility/Loader/FontLoader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace sd {

Font::Font(FT_Face face) : m_face(face) {}

Font::~Font() { FT_Done_Face(m_face); }

const Character &Font::getCharacter(wchar_t ch, int size) {
    CharacterId id(ch, size);
    if (m_characters.count(id) == 0) {
        loadFont(id);
    }
    return m_characters.at(id);
}

void Font::loadFont(const CharacterId &id) {
    FT_Set_Pixel_Sizes(m_face, 0, id.size);
    if (FT_Load_Char(m_face, id.ch, FT_LOAD_RENDER)) {
        SD_CORE_WARN("Failed to load glyph!");
        return;
    }
    Character c;
    c.size =
        glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
    c.bearing =
        glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
    c.advance = m_face->glyph->advance.x;
    c.texture = Texture::create(
        m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, 1,
        TextureType::TEX_2D, TextureFormat::ALPHA, TextureFormatType::UBYTE,
        TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_LINEAR, m_face->glyph->bitmap.buffer);
    m_characters[id] = c;
}

}  // namespace sd
