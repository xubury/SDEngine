#ifndef SD_MODEL_ASSET_HPP
#define SD_MODEL_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Asset/TextureAsset.hpp"
#include "AssetStorage.hpp"
#include "Loader/ModelLoader.hpp"

namespace SD {

class ModelAsset : public Asset {
   public:
    ModelAsset() : m_textures_cnt(0) { m_model = CreateRef<Model>(); };

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override
    {
        archive(*this);
        auto &storage = AssetStorage::Get();
        for (const auto &obj : m_textures) {
            storage.SerializeAsset(obj, archive);
        }
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override
    {
        archive(*this);
        m_textures.resize(m_textures_cnt);
        auto &storage = AssetStorage::Get();
        for (auto &obj : m_textures) {
            obj =
                dynamic_cast<TextureAsset *>(storage.DeserializeAsset(archive));
        }

        if (m_model_path.empty()) {
            throw Exception("Empty model asset");
        }
        m_model = ModelLoader::LoadModel(storage.GetAbsolutePath(m_model_path));
    }

    void Import(const Ref<Model> &model)
    {
        m_model = model;
        auto &storage = AssetStorage::Get();
        m_model_path = storage.GetRelativePath(m_model->GetPath());
        // import materials as asset
        for (const auto &[path, texture] : m_model->GetTextures()) {
            const std::string name =
                fmt::format("{}_{}", GetName(), m_textures_cnt);
            auto asset = storage.CreateAsset<TextureAsset>(name);
            asset->Import(texture);
            m_textures.push_back(asset);
            ++m_textures_cnt;
        }
    }

    const std::string &GetModelPath() const { return m_model_path; }

    Model *GetModel() { return m_model.get(); }

    SERIALIZE(m_model_path, m_textures_cnt)
   private:
    std::string m_model_path;
    Ref<Model> m_model;

    int m_textures_cnt;
    // the reference of texture assets created by loading  model file
    std::vector<Asset *> m_textures;
};

}  // namespace SD
#endif
