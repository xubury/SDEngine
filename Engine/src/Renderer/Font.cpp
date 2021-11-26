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

void Font::LoadGlyph(uint8_t size) {
    if (m_font_atlas.count(size)) return;
    static const char32_t ranges[] = {
        0x0020, 0x00FF,  // Basic Latin + Latin Supplement
        0x0102, 0x0103,  // Vietnamese
        0x0110, 0x0111,  // Vietnamese
        0x0128, 0x0129,  // Vietnamese
        0x0168, 0x0169,  // Vietnamese
        0x01A0, 0x01A1,  // Vietnamese
        0x01AF, 0x01B0,  // Vietnamese
        0x1EA0, 0x1EF9,  // Vietnamese
        0x0E00, 0x0E7F,  // Thai
        0x2000, 0x206F,  // General Punctuation
        0x0400, 0x052F,  // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF,  // Cyrillic Extended-A
        0xA640, 0xA69F,  // Cyrillic Extended-B
        0x3000, 0x30FF,  // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF,  // Katakana Phonetic Extensions
        0xFF00, 0xFFEF,  // Half-width characters
        0xFFFD, 0xFFFD,  // Invalid
        0x4E00, 0x9FFF,  // CJK Ideograms
        0};
    FT_Set_Pixel_Sizes(m_face, 0, size);
    for (size_t i = 0; ranges[i] != 0; i += 2) {
        LoadRangedGlyph(size, ranges[i], ranges[i + 1] + 1);
    }
    m_font_atlas.emplace(size);
}

void Font::LoadRangedGlyph(uint8_t size, char32_t start, char32_t end) {
    const int64_t NUM_GLYPHS = end - start;
    if (NUM_GLYPHS < 0) return;

    const uint32_t HEIGHT = m_face->size->metrics.height >> 6;
    const uint32_t max_dim = (HEIGHT + 1) * std::ceil(std::sqrt(NUM_GLYPHS));
    uint32_t tex_size = 1;
    while (tex_size < max_dim) tex_size <<= 1;

    char *pixels = new char[tex_size * tex_size];
    uint32_t x = 0;
    uint32_t y = 0;
    auto glyph = Texture::Create(tex_size, tex_size, 1, TextureType::TEX_2D,
                                 TextureFormat::ALPHA, TextureFormatType::UBYTE,
                                 TextureWrap::BORDER, TextureFilter::LINEAR,
                                 TextureMipmapFilter::LINEAR_LINEAR, nullptr);
    for (char32_t ch = start; ch < end; ++ch) {
        if (FT_Load_Char(m_face, ch,
                         FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT |
                             FT_LOAD_TARGET_LIGHT)) {
            SD_CORE_WARN("[Font::LoadGlpyph] Failed to load character!");
            continue;
        }

        FT_Bitmap *bmp = &m_face->glyph->bitmap;
        if (x + bmp->width >= tex_size) {
            x = 0;
            y += HEIGHT + 1;
        }

        for (uint32_t row = 0; row < bmp->rows; ++row) {
            std::copy(bmp->buffer + row * bmp->pitch,
                      bmp->buffer + (row + 1) * bmp->pitch,
                      pixels + (y + row) * tex_size + x);
        }

        CharacterId id(ch, size);
        Character &c = m_characters[id];
        c.bearing =
            glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
        c.size = glm::ivec2(bmp->width, bmp->rows);
        c.advance = m_face->glyph->advance.x >> 6;

        c.texCoords[0] =
            glm::vec2(static_cast<float>(x) / tex_size,
                      static_cast<float>(y + bmp->rows) / tex_size);
        c.texCoords[1] =
            glm::vec2(static_cast<float>(x + bmp->width) / tex_size,
                      static_cast<float>(y) / tex_size);
        c.glyph = glyph;

        x += bmp->width + 1;
    }
    glyph->SetPixels(tex_size, tex_size, pixels);
    delete[] pixels;
}

}  // namespace SD
