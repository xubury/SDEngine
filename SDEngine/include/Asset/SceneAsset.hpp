#ifndef SD_SCENE_ASSET_HPP
#define SD_SCENE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/SceneLoader.hpp"

namespace SD {

class SceneAsset : public Asset {
   public:
    SceneAsset() { m_scene = CreateRef<Scene>(); };
    void Init() override {
        try {
            if (!m_scene_path.empty()) {
                m_scene = SceneLoader::LoadScene(GetAbsolutePath(m_scene_path));
            }
            Asset::Init();
        } catch (const Exception &e) {
            SD_CORE_WARN(e.what());
        }
    }

    void SetScenePath(const std::string &path) {
        const std::filesystem::path dir = GetDirectory();
        m_scene_path = std::filesystem::relative(path, dir).generic_string();
    }

    Scene *GetScene() { return m_scene.get(); }

    SERIALIZE(m_scene_path)

   private:
    std::string m_scene_path;
    Ref<Scene> m_scene;
};

}  // namespace SD
#endif
