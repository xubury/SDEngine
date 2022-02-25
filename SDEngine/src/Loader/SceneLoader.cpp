#include "Loader/SceneLoader.hpp"
#include "ECS/Scene.hpp"

namespace SD {

Ref<void> SceneLoader::LoadAsset(const std::string &path) {
    Ref<Scene> scene = CreateRef<Scene>();
    std::ifstream is(path, std::ios::binary);
    cereal::PortableBinaryInputArchive archive(is);
    scene->Deserialize(archive);
    return scene;
}

bool SceneLoader::SaveAsset(const Ref<void> &asset, const std::string &path) {
    std::ofstream os(path, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(os);
    Ref<Scene> scene = std::static_pointer_cast<Scene>(asset);
    if (scene) {
        scene->Serialize(archive);
        return true;
    } else {
        return false;
    }
}

}  // namespace SD
