#ifndef SD_MATERIAL_LOADER_HPP
#define SD_MATERIAL_LOADER_HPP

#include "Utils/AssetLoader.hpp"
#include "Graphics/Material.hpp"

namespace sd {

class MaterialLoader : public AssetLoader<Material> {
   public:
    MaterialLoader(AssetManager &manager);

    Ref<Material> loadAsset(const std::string &filePath) override;
};

}  // namespace sd

#endif /* SD_MATERIAL_LOADER_HPP */
