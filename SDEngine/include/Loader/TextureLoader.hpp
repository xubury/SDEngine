#ifndef SD_SPRITE_LOADER_HPP
#define SD_SPRITE_LOADER_HPP

#include "Graphics/Texture.hpp"
#include "Utility/Base.hpp"
#include "Loader/Export.hpp"

namespace SD {

class ResourceRegistry;

class SD_LOADER_API TextureLoader {
   public:
    Ref<Texture> Load(const std::string &path, TextureWrap wrap,
                      TextureMinFilter min_filter, TextureMagFilter mag_filter,
                      MipmapMode mode, ResourceRegistry &registry);
    // static void WriteToFile(const Texture &texture, const std::string &path);
};

}  // namespace SD

#endif
