#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Asset/Asset.hpp"
#include "Renderer/Model.hpp"

namespace sd {

class SD_API ModelLoader : public AssetLoader<Model> {
   private:
    friend class AssetManager;

    ModelLoader() = default;

   public:
    Ref<void> loadAsset(const std::string &filePath) override;
};

}  // namespace sd

#endif /* SD_MODEL_LOADER_HPP */
