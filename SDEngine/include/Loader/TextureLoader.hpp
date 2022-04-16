#ifndef SD_SPRITE_LOADER_HPP
#define SD_SPRITE_LOADER_HPP

#include "Graphics/Texture.hpp"
#include "Utility/Base.hpp"
#include "Loader/Export.hpp"

namespace SD {

class ResourceRegistry;

class SD_LOADER_API TextureLoader {
   public:
    Ref<Texture> Load(const std::string &path, const TextureParameter &param,
                      ResourceRegistry &registry);
    // static void WriteToFile(const Texture &texture, const std::string &path);
};

}  // namespace SD

#endif
