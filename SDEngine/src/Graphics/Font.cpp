#include "Graphics/Font.hpp"

namespace SD {

Font::Font(uint32_t pixel_height) : m_pixel_height(pixel_height) {}

const Character &Font::GetCharacter(char32_t utf32) const
{
    if (m_characters.count(utf32) == 0) {
        SD_CORE_ASSERT(false, "Error, character not loaded");
    }
    return m_characters.at(utf32);
}

void Font::SetCharacter(char32_t utf32, const Character &ch)
{
    m_characters[utf32] = ch;
}

void Font::SetCharacter(char32_t utf32, Character &&ch)
{
    m_characters[utf32] = std::move(ch);
}

}  // namespace SD
