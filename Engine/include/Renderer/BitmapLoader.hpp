#ifndef SD_BITMAP_LOADER_HPP
#define SD_BITMAP_LOADER_HPP

#include "Renderer/Export.hpp"
#include "Renderer/Bitmap.hpp"
#include "Asset/Asset.hpp"

namespace SD {

class SD_RENDERER_API BitmapLoader : public AssetLoader {
   public:
    BitmapLoader(AssetManager &manager);
    ~BitmapLoader();

    Ref<void> LoadAsset(const std::string &path) override;
};

}  // namespace SD

#endif
