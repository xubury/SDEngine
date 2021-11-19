#ifndef SD_FONT_HPP
#define SD_FONT_HPP

#include "Utility/Base.hpp"
#include "Graphics/Texture.hpp"

#include <glm/glm.hpp>

typedef struct FT_FaceRec_ *FT_Face;

namespace SD {

struct SD_API Character {
    Character() = default;
    Ref<Texture> texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
};

struct CharacterId {
    CharacterId() = default;
    CharacterId(char32_t ch, uint8_t size) : ch(ch), size(size) {}
    bool operator<(const CharacterId &rhs) const {
        return ch < rhs.ch && size < rhs.size;
    }
    bool operator==(const CharacterId &rhs) const {
        return ch == rhs.ch && size == rhs.size;
    }
    char32_t ch;
    uint8_t size;
};

struct CharacterHash {
    std::size_t operator()(const CharacterId &chId) const {
        auto h1 = std::hash<char32_t>{}(chId.ch);
        auto h2 = std::hash<int>{}(chId.size);
        return h1 ^ h2;
    }
};

class SD_API Font {
   public:
    Font(FT_Face face);
    ~Font();

    Font(const Font &) = delete;
    Font operator=(const Font &) = delete;

    const Character &GetCharacter(char32_t ch, uint8_t size);

   private:
    void LoadFont(const CharacterId &id);

    FT_Face m_face;
    std::unordered_map<CharacterId, Character, CharacterHash> m_characters;
};

}  // namespace SD

#endif /* SD_FONT_HPP */
