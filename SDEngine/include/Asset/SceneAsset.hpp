#ifndef SD_SCENE_ASSET_HPP
#define SD_SCENE_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Loader/SceneLoader.hpp"

namespace SD {

class SceneAsset : public Asset {
   public:
    SceneAsset() { m_scene = CreateRef<Scene>(); };

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override {
        m_scene->Serialize(archive);
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override {
        m_scene->Deserialize(archive);
    }

    Scene *GetScene() { return m_scene.get(); }

   private:
    Ref<Scene> m_scene;
};

}  // namespace SD
#endif
