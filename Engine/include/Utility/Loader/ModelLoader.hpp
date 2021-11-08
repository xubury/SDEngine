#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Utility/Loader/AssetLoader.hpp"
#include "Renderer/Model.hpp"

namespace sd {

class SD_API ModelLoader : public AssetLoader<Model> {
   public:
    ModelLoader(AssetManager &manager);

    Ref<Model> loadAsset(const std::string &filePath) override;
};

}  // namespace sd

#endif /* SD_MODEL_LOADER_HPP */
