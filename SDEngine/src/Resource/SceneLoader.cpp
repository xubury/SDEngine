#include "Resource/SceneLoader.hpp"

namespace SD {

// Ref<Scene> SceneLoader::LoadFromFile(const std::string &path)
// {
//     Ref<Scene> scene = CreateRef<Scene>();
//     std::ifstream is(path, std::ios::binary);
//     cereal::PortableBinaryInputArchive archive(is);
//     scene->Deserialize(archive);
//     return scene;
// }

// void SceneLoader::WriteToFile(const Scene &scene, const std::string &path)
// {
//     std::ofstream os(path, std::ios::binary);
//     cereal::PortableBinaryOutputArchive archive(os);
//     scene.Serialize(archive);
// }

}  // namespace SD
