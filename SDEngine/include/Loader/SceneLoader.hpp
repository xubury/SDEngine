#ifndef SD_SCENE_LOADER_HPP
#define SD_SCENE_LOADER_HPP

#include "Loader/Export.hpp"
#include "ECS/Scene.hpp"

namespace SD {

class SD_LOADER_API SceneLoader {
   public:
    static Ref<Scene> LoadFromFile(const std::string &path);
    static void WriteToFile(const Scene &scene, const std::string &path);
};

}  // namespace SD

#endif  // !SD_SCENE_LOADER_HPP
