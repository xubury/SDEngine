#ifndef SD_SPRITE_LOADER_HPP
#define SD_SPRITE_LOADER_HPP

#include "Graphics/Texture.hpp"
#include "Utility/Base.hpp"
#include "Loader/Export.hpp"

#include "Resource/ResourceLoader.hpp"

namespace SD {

class SD_LOADER_API TextureLoader
    : public ResourceLoader<TextureLoader, Texture> {
   public:
    Ref<Texture> Load(const std::string &path) const;
    // static void WriteToFile(const Texture &texture, const std::string &path);
};

}  // namespace SD

#endif
