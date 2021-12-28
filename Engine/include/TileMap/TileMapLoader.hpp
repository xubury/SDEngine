#ifndef SD_TILE_MAP_LOADER_HPP
#define SD_TILE_MAP_LOADER_HPP

#include "TileMap//Export.hpp"
#include "Asset/Asset.hpp"

namespace SD {

class SD_TILE_MAP_API TileMapLoader : public AssetLoader {
   public:
    using AssetLoader::AssetLoader;

    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif
