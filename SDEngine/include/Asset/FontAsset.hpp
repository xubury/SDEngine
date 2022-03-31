#ifndef SD_FONT_ASSET_HPP
#define SD_FONT_ASSET_HPP

#include "Asset/Export.hpp"
#include "Asset/Asset.hpp"
#include "AssetStorage.hpp"
#include "Loader/FontLoader.hpp"

namespace SD {

class FontAsset : public Asset {
   public:
    FontAsset() : m_pixel_height(20){};

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override
    {
        archive(*this);
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override
    {
        archive(*this);
        auto &storage = AssetStorage::Get();
        m_font = FontLoader::LoadFont(storage.GetAbsolutePath(m_font_path),
                                      m_pixel_height);
    }

    Font *GetFont() { return m_font.get(); }

    void Import(const std::string &path, int32_t height)
    {
        auto &storage = AssetStorage::Get();
        m_pixel_height = height;
        m_font_path = storage.GetRelativePath(path);
        m_font = FontLoader::LoadFont(path, m_pixel_height);
    }

    SERIALIZE(m_pixel_height, m_font_path)

   private:
    Ref<Font> m_font;
    int32_t m_pixel_height;
    std::string m_font_path;
};

}  // namespace SD

#endif
