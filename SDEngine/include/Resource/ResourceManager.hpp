#ifndef SD_RESOURCE_MANAGER_HPP
#define SD_RESOURCE_MANAGER_HPP

#include "Resource/Export.hpp"
#include "Resource/Resource.hpp"

namespace SD {

class SD_RESOURCE_API ResourceManager {
   public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ImageCache images;

    TextureCache textures;

    ModelCache models;

    ShaderCache shaders;
};

}  // namespace SD

#endif
