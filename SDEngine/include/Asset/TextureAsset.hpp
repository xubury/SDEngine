#ifndef SD_TEXTURE_ASSET_HPP
#define SD_TEXTURE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

class TextureAsset : public Asset {
   public:
    void *LoadFromFile(const std::string &path) {
        *this = Asset::LoadArchiveFromFile<TextureAsset>(path);
        if (m_type == TextureType::TEX_2D) {
            m_texture = TextureLoader::LoadTexture2D(m_texture_path[0]);
        } else if (m_type == TextureType::TEX_CUBE) {
            m_texture = TextureLoader::LoadTextureCube(m_texture_path);
        }
        return this;
    }

    Texture *GetTexture() { return m_texture.get(); }

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
