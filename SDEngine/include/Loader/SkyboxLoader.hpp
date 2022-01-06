#ifndef SD_SKYBOX_LOADER_HPP
#define SD_SKYBOX_LOADER_HPP

#include "Loader/Export.hpp"
#include "Renderer/Skybox.hpp"
#include "Asset/AssetManager.hpp"

namespace SD {

class SD_LOADER_API SkyboxLoader : public AssetLoader {
   public:
    using AssetLoader::AssetLoader;

    Ref<void> LoadAsset(const std::string& path) override;

   private:
    void ParseStream(std::istream& stream,
                     const std::filesystem::path& directory, Skybox& skybox);
};

}  // namespace SD

#endif  // !SD_SKYBOX_LOADER_HPP
