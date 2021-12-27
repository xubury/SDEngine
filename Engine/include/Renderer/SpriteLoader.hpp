#ifndef SD_TILE_MAP_LOADER_HPP
#define SD_TILE_MAP_LOADER_HPP

#include "Renderer/Export.hpp"
#include "Asset/Asset.hpp"

namespace SD {

class SD_RENDERER_API SpriteLoader : public AssetLoader {
   public:
    using AssetLoader::AssetLoader;

    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif
