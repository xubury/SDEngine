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

void Font::LoadChineseGlyph(uint8_t size) {
    if (m_font_atlas.count(size)) return;
    static const char32_t ranges[] = {
        0x0020, 0x00FF,  // Basic Latin + Latin Supplement
        0x2000, 0x206F,  // General Punctuation
        0x3000, 0x30FF,  // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF,  // Katakana Phonetic Extensions
        0xFF00, 0xFFEF,  // Half-width characters
        0xFFFD, 0xFFFD,  // Invalid
        0x4E00, 0x9FFF,  // CJK Ideograms
        // 0x3400, 0x4DBF,    // Rare
        // 0x20000, 0x2A6DF,  // Rare, historic
        // 0x2A700, 0x2B73F,  // Rare, historic
        // 0x2B740, 0x2B81F,  // Uncommon, some in current use
        // 0x2B820, 0x2CEAF,  // Rare, historic
        // 0xF900, 0xFAFF,    // Duplicates, unifiable variants, corporate
        //                    // characters
        // 0x2F800, 0x2FA1F,  // Unifiable variants,
        0};
    FT_Set_Pixel_Sizes(m_face, 0, size);
    for (size_t i = 0; ranges[i] != 0; i += 2) {
        LoadGlpyph(size, ranges[i], ranges[i + 1] + 1);
    }
    m_font_atlas.emplace(size);
    FT_Done_Face(m_face);
}

Ref<Texture> Font::LoadGlpyph(uint8_t size, char32_t start, char32_t end) {
    const int64_t NUM_GLYPHS = end - start;
    if (NUM_GLYPHS < 0) return nullptr;

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

        c.texCoord[0] = glm::vec2(static_cast<float>(x) / tex_size,
                                  static_cast<float>(y + bmp->rows) / tex_size);
        c.texCoord[1] = glm::vec2(static_cast<float>(x + bmp->width) / tex_size,
                                  static_cast<float>(y + bmp->rows) / tex_size);
        c.texCoord[2] = glm::vec2(static_cast<float>(x + bmp->width) / tex_size,
                                  static_cast<float>(y) / tex_size);
        c.texCoord[3] = glm::vec2(static_cast<float>(x) / tex_size,
                                  static_cast<float>(y) / tex_size);
        c.texture = glyph;

        x += bmp->width + 1;
    }
    glyph->SetPixels(tex_size, tex_size, pixels);
    delete[] pixels;
    return glyph;
}

}  // namespace SD
