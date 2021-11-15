#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Asset/Asset.hpp"
#include "Renderer/Model.hpp"

namespace SD {

class SD_API ModelLoader : public AssetLoaderBase {
   private:
    friend class AssetManager;

    using AssetLoaderBase::AssetLoaderBase;

   public:
    Ref<void> loadAsset(const std::string &filePath) override;
};

}  // namespace SD

#endif /* SD_MODEL_LOADER_HPP */
