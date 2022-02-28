#ifndef SD_MODEL_ASSET_HPP
#define SD_MODEL_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/ModelLoader.hpp"

namespace SD {

class ModelAsset : public Asset {
   public:
    ModelAsset() = default;
    void LoadFromFile(const std::string &path) override {
        LoadArchiveFromFile<ModelAsset>(path, this);

        m_model = ModelLoader::LoadModel(m_model_path);
    }

    Model *GetModel() { return m_model.get(); }

    SERIALIZE(m_model_path)
   private:
    Ref<Model> m_model;
    std::string m_model_path;
};

}  // namespace SD
#endif
