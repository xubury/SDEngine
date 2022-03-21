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

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override
    {
        archive(*this);
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override
    {
        archive(*this);
        auto &storage = AssetStorage::Get();

        m_model = ModelLoader::LoadModel(storage.GetAbsolutePath(m_model_path));
    }

    void Import(const std::string &full_path)
    {
        m_model = ModelLoader::LoadModel(full_path);
        auto &storage = AssetStorage::Get();
        m_model_path = storage.GetRelativePath(full_path);
    }

    const std::string &GetModelPath() const { return m_model_path; }

    Model *GetModel() { return m_model.get(); }

    SERIALIZE(m_model_path)
   private:
    std::string m_model_path;
    Ref<Model> m_model;
};

}  // namespace SD
#endif
