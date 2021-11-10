#include "Asset/FontLoader.hpp"
#include "Utility/Log.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace sd {

FontLoader::FontLoader() {
    if (FT_Init_FreeType(&m_ft)) {
        SD_CORE_ERROR("Could not init Freetype library!");
    }
}

FontLoader::~FontLoader() {
    // FT_Done_FreeType(m_ft);
}

Ref<void> FontLoader::loadAsset(const std::string &filePath) {
    FT_Face face;
    Ref<Font> font;
    if (FT_New_Face(m_ft, filePath.c_str(), 0, &face)) {
        SD_CORE_ERROR("Failed to load font!");
    } else {
        font = createRef<Font>(face);
    }
    return font;
}

}  // namespace sd
