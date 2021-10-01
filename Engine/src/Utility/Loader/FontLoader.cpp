#include "Utility/Loader/FontLoader.hpp"
#include "Utility/SDLHelper.hpp"
#include "Graphics/Font.hpp"
#include <SDL_ttf.h>

namespace sd {

FontLoader::FontLoader(AssetManager &manager) : AssetLoader<Font>(manager) {
    TTF_Init();
}

FontLoader::~FontLoader() { TTF_Quit(); }

Ref<Font> FontLoader::loadAsset(const std::string &filePath) {
    Ref<Font> font;
    TTF_Font *ttf = TTF_OpenFont(filePath.c_str(), 25);
    if (ttf == nullptr) {
        SD_CORE_ERROR("Failed to load font! SDL_ttf Error: {}", TTF_GetError());
    } else {
        font = createRef<Font>();
        int minX = 0;
        int minY = 0;
        int maxX = 0;
        int maxY = 0;
        int advance = 0;
        SDL_Color color = {0xff, 0xff, 0xff, 0xff};
        Character c;
        for (wchar_t ch = 20; ch < 128; ++ch) {
            if (ch == 0 || ch == 8203 || ch == 65279) continue;  // zero width space
            if (TTF_GlyphMetrics(ttf, ch, &minX, &maxX, &minY, &maxY,
                                 &advance) == -1) {
                SD_CORE_ERROR("TTF Error: {}", TTF_GetError());
                break;
            }
            SDL_Surface *surface = TTF_RenderGlyph_Solid(ttf, ch, color);
            if (surface == nullptr) {
                SD_CORE_ERROR("TTF Error: {}. Unicode: \"{}\"",
                              TTF_GetError(), ch);
                continue;
            }
            c.texture = surfaceToTexture(surface);
            c.size = glm::ivec2(maxX - minX, maxY - minY);
            c.bearing = glm::ivec2(minX, maxY);
            c.advance = advance;
            font->setCharacter(ch, c);
        }

        TTF_CloseFont(ttf);
    }
    return font;
}

}  // namespace sd
