#include "Asset/FontLoader.hpp"
#include "Asset/Font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace SD {

FontLoader::FontLoader(AssetManager &manager) : AssetLoader(manager) {
    if (FT_Init_FreeType(&m_ft)) {
        SD_CORE_ERROR("Could not init Freetype library!");
    }
}

FontLoader::~FontLoader() { FT_Done_FreeType(m_ft); }

Ref<void> FontLoader::LoadAsset(const std::string &path) {
    FT_Face face;
    Ref<Font> font;
    SD_CORE_TRACE("Loading font form: {}...", path);
    if (FT_New_Face(m_ft, path.c_str(), 0, &face)) {
        SD_CORE_ERROR("Failed to load font {}!", path);
    } else {
        font = CreateRef<Font>(face);
    }
    return font;
}

}  // namespace SD
