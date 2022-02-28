#ifndef SD_TEXTURE_ASSET_HPP
#define SD_TEXTURE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

class TextureAsset : public Asset {
   public:
    TextureAsset() : m_type(TextureType::TEX_2D), m_texture_path(1) {}
    ~TextureAsset() { SD_CORE_TRACE("deleting asset"); }

    void LoadFromFile(const std::string &path) override {
        Asset::LoadArchiveFromFile<TextureAsset>(path, this);
        try {
            if (m_type == TextureType::TEX_2D) {
                // TODO:fix the path problem
                m_texture = TextureLoader::LoadTexture2D(m_texture_path[0]);
            } else if (m_type == TextureType::TEX_CUBE) {
                m_texture = TextureLoader::LoadTextureCube(m_texture_path);
            }
        } catch (const Exception &e) {
            SD_CORE_WARN(e.what());
        }
    }

    Texture *GetTexture() { return m_texture.get(); }

    void SetTexturePath(const std::string &path) {
        m_texture_path[0] =
            std::filesystem::path(path).lexically_relative(m_directory);
    }
    std::string GetTexturePath() const { return m_texture_path[0]; }

    SERIALIZE(m_type, m_wrap, m_filter, m_min_filter, m_texture_path)

   private:
    Ref<Texture> m_texture;
    TextureType m_type;
    TextureWrap m_wrap;
    TextureMagFilter m_filter;
    TextureMinFilter m_min_filter;
    std::vector<std::string> m_texture_path;
};

}  // namespace SD

#endif
