#include "Asset/FontLoader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace SD {

FontLoader::FontLoader(AssetManager &manager) : AssetLoaderBase(manager) {
    if (FT_Init_FreeType(&m_ft)) {
        SD_CORE_ERROR("Could not init Freetype library!");
    }
}

FontLoader::~FontLoader() {
    // FT_Done_FreeType(m_ft);
}

Ref<void> FontLoader::LoadAsset(const std::string &filePath) {
    FT_Face face;
    Ref<Font> font;
    SD_CORE_TRACE("Loading font form: {}...", filePath);
    if (FT_New_Face(m_ft, filePath.c_str(), 0, &face)) {
        SD_CORE_ERROR("Failed to load font {}!", filePath);
    } else {
        font = CreateRef<Font>(face);
    }
    return font;
}

}  // namespace SD
