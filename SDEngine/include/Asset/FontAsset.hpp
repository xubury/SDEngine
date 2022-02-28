#ifndef SD_FONT_ASSET_HPP
#define SD_FONT_ASSET_HPP

#include "Asset/Export.hpp"
#include "Asset/Asset.hpp"

#include "Loader/FontLoader.hpp"

namespace SD {

class FontAsset : public Asset {
   public:
    FontAsset() : m_pixel_height(20){};
    void LoadFromFile(const std::string &path) override {
        LoadArchiveFromFile<FontAsset>(path, this);
        m_font = FontLoader::LoadFont(m_font_path, m_pixel_height);
    }

    Font *GetFont() { return m_font.get(); }

    SERIALIZE(m_pixel_height, m_font_path)

   private:
    Ref<Font> m_font;
    int32_t m_pixel_height;
    std::string m_font_path;
};

}  // namespace SD

#endif
