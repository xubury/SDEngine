#ifndef SD_MODEL_ASSET_HPP
#define SD_MODEL_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Asset/TextureAsset.hpp"
#include "AssetStorage.hpp"
#include "Loader/ModelLoader.hpp"

namespace SD {

class ModelAsset : public Asset {
   public:
    ModelAsset() { m_model = CreateRef<Model>(); };

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override {
        archive(*this);
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override {
        archive(*this);

        if (m_model_path.empty()) {
            throw Exception("Empty model asset");
        }

        auto &storage = AssetStorage::Get();
        m_model = ModelLoader::LoadModel(storage.GetAbsolutePath(m_model_path));
        ImportModelMaterials();
    }

    void Import(const Ref<Model> &model) {
        m_model = model;
        auto &storage = AssetStorage::Get();
        m_model_path = storage.GetRelativePath(m_model->GetPath());
        ImportModelMaterials();
    }

    const std::string &GetModelPath() const { return m_model_path; }

    Model *GetModel() { return m_model.get(); }

    SERIALIZE(m_model_path)
   private:
    void ImportModelMaterials() {
        auto &storage = AssetStorage::Get();
        int cnt = 0;
        for (const auto &[path, texture] : m_model->GetImportedTexture()) {
            const std::string name = fmt::format("{}_{}", GetName(), cnt);
            auto asset = storage.CreateAsset<TextureAsset>(name);
            asset->Import(texture);
            ++cnt;
        }
    }

    Ref<Model> m_model;
    std::string m_model_path;
};

}  // namespace SD
#endif
