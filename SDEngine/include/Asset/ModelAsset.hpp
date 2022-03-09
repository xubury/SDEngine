#ifndef SD_MODEL_ASSET_HPP
#define SD_MODEL_ASSET_HPP

#include "Asset/Asset.hpp"
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
    }

    void Import(const std::string &path) {
        auto &storage = AssetStorage::Get();
        m_model_path = storage.GetRelativePath(path);
        m_model = ModelLoader::LoadModel(path);
    }

    const std::string &GetModelPath() const { return m_model_path; }

    Model *GetModel() { return m_model.get(); }

    SERIALIZE(m_model_path)
   private:
    Ref<Model> m_model;
    std::string m_model_path;
};

}  // namespace SD
#endif
