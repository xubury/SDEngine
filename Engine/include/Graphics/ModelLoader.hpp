#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Utils/AssetLoader.hpp"
#include "Graphics/Model.hpp"

namespace sd {

class SD_API ModelLoader : public AssetLoader<Model> {
   public:
    ModelLoader(AssetManager &manager);

    Ref<Model> loadAsset(const std::string &filePath) override;
};

}  // namespace sd

#endif /* SD_MODEL_LOADER_HPP */
