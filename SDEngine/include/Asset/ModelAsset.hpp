#ifndef SD_MODEL_ASSET_HPP
#define SD_MODEL_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/ModelLoader.hpp"

namespace SD {

class ModelAsset : public Asset {
   public:
    ModelAsset() = default;
    void Init() override {
        try {
            m_model = ModelLoader::LoadModel(GetAbsolutePath(m_model_path));
            Asset::Init();
        } catch (const Exception &e) {
            SD_CORE_WARN(e.what());
        }
    }

    void SetModelPath(const std::string &path) {
        const std::filesystem::path dir =
            std::filesystem::path(m_path).parent_path();
        m_model_path = std::filesystem::relative(path, dir).generic_string();
    }
    Model *GetModel() { return m_model.get(); }

    SERIALIZE(m_model_path)
   private:
    Ref<Model> m_model;
    std::string m_model_path;
};

}  // namespace SD
#endif
