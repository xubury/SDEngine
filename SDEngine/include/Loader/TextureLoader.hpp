#ifndef SD_SPRITE_LOADER_HPP
#define SD_SPRITE_LOADER_HPP

#include "Graphics/Texture.hpp"
#include "Utility/Base.hpp"
#include "Loader/Export.hpp"

namespace SD {

class SD_LOADER_API TextureLoader {
   public:
    static Ref<Texture> LoadTexture2D(const std::string &path);
    static Ref<Texture> LoadTextureCube(const std::vector<std::string> &path);
};

}  // namespace SD

#endif
