#include "Renderer/Font.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace SD {

Font::Font(FT_Face face) : m_face(face) {}

Font::~Font() { FT_Done_Face(m_face); }

const Character &Font::GetCharacter(char32_t ch, uint8_t size) {
    CharacterId id(ch, size);
    if (m_characters.count(id) == 0) {
        SD_CORE_ASSERT(false, "Error, character not loaded");
    }
    return m_characters.at(id);
}

void Font::LoadASCIIGlyph(uint8_t size) {
    if (m_ascii_glyph.count(size)) return;

    FT_Set_Pixel_Sizes(m_face, 0, size);

    const uint32_t NUM_GLYPHS = 128;
    const uint32_t tex_size = ((m_face->size->metrics.height >> 6) + 1) *
                              std::ceil(std::sqrt(NUM_GLYPHS));

    char *pixels = new char[tex_size * tex_size];
    uint32_t x = 0;
    uint32_t y = 0;
    m_ascii_glyph[size] = Texture::Create(
        tex_size, tex_size, 1, TextureType::TEX_2D, TextureFormat::ALPHA,
        TextureFormatType::UBYTE, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_LINEAR, nullptr);
    for (char32_t ch = 0; ch < NUM_GLYPHS; ++ch) {
        if (std::iscntrl(ch)) continue;

        if (FT_Load_Char(m_face, ch,
                         FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT |
                             FT_LOAD_TARGET_LIGHT)) {
            SD_CORE_WARN("[Font::LoadASCIIGlyph] Failed to load character!");
            continue;
        }
        FT_Bitmap *bmp = &m_face->glyph->bitmap;

        if (x + bmp->width >= tex_size) {
            x = 0;
            y += (m_face->size->metrics.height >> 6) + 1;
        }

        for (uint32_t row = 0; row < bmp->rows; ++row) {
            std::copy(bmp->buffer + row * bmp->width,
                      bmp->buffer + (row + 1) * bmp->width,
                      pixels + (y + row) * tex_size + x);
        }

        CharacterId id(ch, size);
        Character &c = m_characters[id];
        c.bearing =
            glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
        c.size = glm::ivec2(bmp->width, bmp->rows);
        c.advance = m_face->glyph->advance.x >> 6;

        c.texCoord[0] = glm::vec2(static_cast<float>(x) / tex_size,
                                  static_cast<float>(y + bmp->rows) / tex_size);
        c.texCoord[1] = glm::vec2(static_cast<float>(x + bmp->width) / tex_size,
                                  static_cast<float>(y + bmp->rows) / tex_size);
        c.texCoord[2] = glm::vec2(static_cast<float>(x + bmp->width) / tex_size,
                                  static_cast<float>(y) / tex_size);
        c.texCoord[3] = glm::vec2(static_cast<float>(x) / tex_size,
                                  static_cast<float>(y) / tex_size);
        c.texture = m_ascii_glyph[size];

        x += bmp->width + 1;
    }
    m_ascii_glyph[size]->SetPixels(tex_size, tex_size, pixels);
    delete[] pixels;
}

}  // namespace SD
