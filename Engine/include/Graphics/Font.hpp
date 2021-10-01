#ifndef SD_FONT_HPP
#define SD_FONT_HPP

#include "Utility/Export.hpp"
#include "Graphics/Texture.hpp"
#include <glm/glm.hpp>

namespace sd {

struct SD_API Character {
    Ref<Texture> texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
};

class SD_API Font {
   public:
    void setCharacter(wchar_t ch, const Character &character);
    const Character &getCharacter(wchar_t ch) const;

   private:
    std::unordered_map<wchar_t, Character> m_characters;
};

}  // namespace sd

#endif /* SD_FONT_HPP */
