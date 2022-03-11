#ifndef SD_TEXTURE_ASSET_HPP
#define SD_TEXTURE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "AssetStorage.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

class TextureAsset : public Asset {
   public:
    TextureAsset() {
        m_texture = Texture::Create(TextureSpec(1, 1, 1, 1, TextureType::TEX_2D,
                                                DataFormat::RGB,
                                                DataFormatType::UBYTE));
    }
    ~TextureAsset() { SD_CORE_TRACE("deleting asset"); }

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override {
        archive(*this);
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override {
        archive(*this);
        auto &storage = AssetStorage::Get();
        m_texture = TextureLoader::LoadTexture2D(
            storage.GetAbsolutePath(m_texture_path));
    }
    void Import(const Ref<Texture> &texture) {
        auto &storage = AssetStorage::Get();
        m_texture_path = storage.GetRelativePath(texture->GetPath());
        m_texture = texture;
    }

    Texture *GetTexture() { return m_texture.get(); }

    SERIALIZE(m_wrap, m_filter, m_min_filter, m_texture_path)

   private:
    Ref<Texture> m_texture;
    TextureWrap m_wrap;
    TextureMagFilter m_filter;
    TextureMinFilter m_min_filter;
    std::string m_texture_path;
};

}  // namespace SD

#endif
