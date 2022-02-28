#ifndef SD_SCENE_ASSET_HPP
#define SD_SCENE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/SceneLoader.hpp"

namespace SD {

class SceneAsset : public Asset {
   public:
    SceneAsset() = default;
    void *LoadFromFile(const std::string &path) {
        *this = LoadArchiveFromFile<SceneAsset>(path);
        m_scene = SceneLoader::LoadScene(m_scene_path);
        return this;
    }

    Scene *GetScene() { return m_scene.get(); }

    SERIALIZE(m_scene_path)

   private:
    std::string m_scene_path;
    Ref<Scene> m_scene;
};

}  // namespace SD
#endif
