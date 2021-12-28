#include "Renderer/FontLoader.hpp"
#include "Renderer/Bitmap.hpp"
#include "Utility/Ini.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace SD {

const std::string SECTION_NAME = "Font";
const uint32_t DEFAULT_FONT_HEIGHT = 20;

const char32_t UNICODE_RANGE[] = {
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

FontLoader::FontLoader(AssetManager &manager) : AssetLoader(manager) {
    if (FT_Init_FreeType(&m_ft)) {
        SD_CORE_ERROR("Could not init Freetype library!");
    }
}

FontLoader::~FontLoader() { FT_Done_FreeType(m_ft); }

Ref<void> FontLoader::LoadAsset(const std::string &path) {
    Ref<Font> font;
    SD_CORE_TRACE("Loading font form: {}...", path);
    Ini ini;
    try {
        ini.Load(path);
        std::filesystem::path font_path =
            std::filesystem::path(path).parent_path() /
            ini.Get("Font", "path", path);
        if (FT_New_Face(m_ft, font_path.string().c_str(), 0, &m_face)) {
            SD_CORE_ERROR("Failed to load font {}!", font_path);
        } else {
            uint32_t pixel_height =
                ini.GetInteger("Font", "pixel_height", DEFAULT_FONT_HEIGHT);
            font = CreateRef<Font>(pixel_height);
            FT_Set_Pixel_Sizes(m_face, 0, pixel_height);
            LoadGlyph(*font);
        }
        FT_Done_Face(m_face);
    } catch (const Exception &e) {
        SD_CORE_ERROR("Error loading font file: {}", e.what());
    }
    return font;
}

void FontLoader::LoadGlyph(Font &font) {
    for (size_t i = 0; UNICODE_RANGE[i] != 0; i += 2) {
        LoadRangedGlyph(font, UNICODE_RANGE[i], UNICODE_RANGE[i + 1] + 1);
    }
}

void FontLoader::LoadRangedGlyph(Font &font, char32_t start, char32_t end) {
    const int64_t NUM_GLYPHS = end - start;
    if (NUM_GLYPHS < 0) return;

    const uint32_t HEIGHT = m_face->size->metrics.height >> 6;
    const uint32_t MAX_DIM = (HEIGHT + 1) * std::ceil(std::sqrt(NUM_GLYPHS));
    uint32_t tex_size = 1;
    while (tex_size < MAX_DIM) tex_size <<= 1;

    Bitmap bmp(tex_size, tex_size, 1);
    uint32_t x = 0;
    uint32_t y = 0;
    auto glyph =
        Texture::Create(tex_size, tex_size,
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::ALPHA,
                                    DataFormatType::UBYTE));
    for (char32_t ch = start; ch < end; ++ch) {
        if (FT_Load_Char(m_face, ch,
                         FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT |
                             FT_LOAD_TARGET_LIGHT)) {
            SD_CORE_WARN("[Font::LoadGlpyph] Failed to load character!");
            continue;
        }

        FT_Bitmap *ft_bmp = &m_face->glyph->bitmap;
        if (x + ft_bmp->width >= tex_size) {
            x = 0;
            y += HEIGHT + 1;
        }

        for (uint32_t row = 0; row < ft_bmp->rows; ++row) {
            bmp.SetPixels(x, y + row, ft_bmp->width, 1,
                          ft_bmp->buffer + row * ft_bmp->pitch);
        }

        Character c;
        c.bearing =
            glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
        c.size = glm::ivec2(ft_bmp->width, ft_bmp->rows);
        c.advance = m_face->glyph->advance.x >> 6;

        c.uv[0] = glm::vec2(static_cast<float>(x) / tex_size,
                            static_cast<float>(y) / tex_size);
        c.uv[1] = glm::vec2(static_cast<float>(x + ft_bmp->width) / tex_size,
                            static_cast<float>(y + ft_bmp->rows) / tex_size);
        c.glyph = glyph;

        font.SetCharacter(ch, std::move(c));
        x += ft_bmp->width + 1;
    }

    glyph->SetPixels(0, 0, 0, tex_size, tex_size, 1, bmp.Data());
}

}  // namespace SD
