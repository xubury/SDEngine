#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Asset/Asset.hpp"
#include "Renderer/Model.hpp"

namespace SD {

class SD_API ModelLoader : public AssetLoaderBase {
   public:
    using AssetLoaderBase::AssetLoaderBase;

    Ref<void> LoadAsset(const std::filesystem::path &filename) override;
};

}  // namespace SD

#endif /* SD_MODEL_LOADER_HPP */
