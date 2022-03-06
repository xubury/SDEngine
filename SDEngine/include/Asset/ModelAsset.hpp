#ifndef SD_MODEL_ASSET_HPP
#define SD_MODEL_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/ModelLoader.hpp"

namespace SD {

class ModelAsset : public Asset {
   public:
    ModelAsset() { m_model = CreateRef<Model>(); };
    void Init() override {
        try {
            if (m_model_path != "NONE") {
                m_model = ModelLoader::LoadModel(GetAbsolutePath(m_model_path));
            }
            Asset::Init();
        } catch (const Exception &e) {
            SD_CORE_WARN(e.what());
        }
    }

    void SetModelPath(const std::string &path) {
        const std::filesystem::path dir = GetDirectory();
        m_model_path = std::filesystem::relative(path, dir).generic_string();
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
