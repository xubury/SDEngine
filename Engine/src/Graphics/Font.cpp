#include "Graphics/Font.hpp"

namespace sd {

void Font::setCharacter(wchar_t ch, const Character &character) {
    m_characters[ch] = character;
}

const Character &Font::getCharacter(wchar_t ch) const {
    return m_characters.at(ch);
}

}  // namespace sd
