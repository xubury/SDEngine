#ifndef SD_FONT_HPP
#define SD_FONT_HPP

#include "Utility/Export.hpp"
#include "Graphics/Texture.hpp"
#include <glm/glm.hpp>

typedef struct FT_FaceRec_ *FT_Face;

namespace sd {

struct SD_API Character {
    Character() = default;
    Ref<Texture> texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
};

struct CharacterId {
    CharacterId() = default;
    CharacterId(wchar_t ch, int size) : ch(ch), size(size) {}
    bool operator<(const CharacterId &rhs) const {
        return ch < rhs.ch && size < rhs.size;
    }
    bool operator==(const CharacterId &rhs) const {
        return ch == rhs.ch && size == rhs.size;
    }
    wchar_t ch;
    int size;
};

struct CharacterHash {
    std::size_t operator()(const CharacterId &chId) const {
        auto h1 = std::hash<wchar_t>{}(chId.ch);
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

    const Character &getCharacter(wchar_t ch, int size);

   private:
    void loadFont(const CharacterId &id);

    FT_Face m_face;
    std::unordered_map<CharacterId, Character, CharacterHash> m_characters;

    int m_size;
};

}  // namespace sd

#endif /* SD_FONT_HPP */
