#ifndef SD_FONT_HPP
#define SD_FONT_HPP

#include "Utility/Base.hpp"
#include "Renderer/Export.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

struct SD_RENDERER_API Character {
    Character() = default;
    Ref<Texture> glyph;
    std::array<glm::vec2, 2> uv;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
};

class SD_RENDERER_API Font {
   public:
    Font(uint32_t pixel_height);
    ~Font() = default;

    Font(const Font &) = delete;
    Font operator=(const Font &) = delete;

    uint32_t GetPixelHeight() const { return m_pixel_height; }
    const Character &GetCharacter(char32_t ch) const;

    void SetCharacter(char32_t utf32, const Character &ch);
    void SetCharacter(char32_t utf32, Character &&ch);

   private:
    uint32_t m_pixel_height;
    std::unordered_map<char32_t, Character> m_characters;
};

}  // namespace SD

#endif /* SD_FONT_HPP */
