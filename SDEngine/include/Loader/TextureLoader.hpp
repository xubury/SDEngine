#ifndef SD_SPRITE_LOADER_HPP
#define SD_SPRITE_LOADER_HPP

#include "Graphics/Texture.hpp"
#include "Utility/Base.hpp"
#include "Loader/Export.hpp"

namespace SD {

class SD_LOADER_API TextureLoader {
   public:
    static Ref<Texture> LoadFromFile(const std::string &path);
    static void WriteToFile(const Texture &texture, const std::string &path);
};

}  // namespace SD

#endif
