#ifndef SD_TEXTURE_ASSET_HPP
#define SD_TEXTURE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

class TextureAsset : public Asset {
   public:
    TextureAsset() : m_type(TextureType::TEX_2D), m_texture_path(1) {}
    ~TextureAsset() { SD_CORE_TRACE("deleting asset"); }

    void Init() override {
        std::vector<std::string> pathes(m_texture_path.size());
        for (size_t i = 0; i < m_texture_path.size(); ++i) {
            pathes[i] = GetAbsolutePath(m_texture_path[i]);
        }
        try {
            if (m_type == TextureType::TEX_2D) {
                m_texture = TextureLoader::LoadTexture2D(pathes[0]);
            } else if (m_type == TextureType::TEX_CUBE) {
                m_texture = TextureLoader::LoadTextureCube(pathes);
            }
            Asset::Init();
        } catch (const Exception &e) {
            SD_CORE_WARN(e.what());
        }
    }

    Texture *GetTexture() { return m_texture.get(); }

    void SetTexturePath(const std::string &path) { m_texture_path[0] = path; }

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
