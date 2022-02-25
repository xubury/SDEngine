#ifndef SD_SCENE_LOADER_HPP
#define SD_SCENE_LOADER_HPP

#include "Loader/Export.hpp"
#include "Asset/AssetManager.hpp"

namespace SD {

class SD_LOADER_API SceneLoader : public AssetLoader {
   public:
    using AssetLoader::AssetLoader;

    Ref<void> LoadAsset(const std::string &path) override;

    bool SaveAsset(const Ref<void> &asset, const std::string &path) override;
};

}  // namespace SD

#endif  // !SD_SCENE_LOADER_HPP
