#ifndef SD_TEXTURE_ASSET_HPP
#define SD_TEXTURE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "AssetStorage.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

class TextureAsset : public Asset {
   public:
    TextureAsset()
    {
        m_texture = Texture::Create(1, 1, 0, MultiSampleLevel::X1,
                                    TextureType::Normal, DataFormat::RGB8);
    }
    ~TextureAsset() {}

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override
    {
        archive(*this);
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override
    {
        archive(*this);
        auto &storage = AssetStorage::Get();
        m_texture = TextureLoader::LoadTexture2D(
            storage.GetAbsolutePath(m_texture_path));
        m_texture->SetWrap(m_wrap);
        m_texture->SetMagFilter(m_filter);
        m_texture->SetMinFilter(m_min_filter);
        m_texture->SetMipmapMode(m_mipmap_mode);
    }

    void Import(const Ref<Texture> &texture, const std::string &full_path)
    {
        auto &storage = AssetStorage::Get();
        m_texture_path = storage.GetRelativePath(full_path);
        m_texture = texture;
        m_wrap = m_texture->GetWrap();
        m_filter = m_texture->GetMagFilter();
        m_min_filter = m_texture->GetMinFilter();
        m_mipmap_mode = m_texture->GetMipmapMode();
    }

    Texture *GetTexture() { return m_texture.get(); }

    SERIALIZE(m_wrap, m_filter, m_min_filter, m_mipmap_mode, m_texture_path)

   private:
    Ref<Texture> m_texture;
    TextureWrap m_wrap;
    TextureMagFilter m_filter;
    TextureMinFilter m_min_filter;
    MipmapMode m_mipmap_mode;
    std::string m_texture_path;
};

}  // namespace SD

#endif
