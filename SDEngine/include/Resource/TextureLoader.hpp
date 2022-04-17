#ifndef SD_SPRITE_LOADER_HPP
#define SD_SPRITE_LOADER_HPP

#include "Graphics/Texture.hpp"
#include "Utility/Base.hpp"
#include "Resource/Export.hpp"

namespace SD {

class SD_RESOURCE_API TextureLoader {
   public:
    Ref<Texture> Load(const std::string &path, const TextureParameter &param);
    // static void WriteToFile(const Texture &texture, const std::string &path);
};

}  // namespace SD

#endif
