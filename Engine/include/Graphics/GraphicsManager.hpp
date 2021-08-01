#ifndef SD_GRAPHICS_MANAGER_HPP
#define SD_GRAPHICS_MANAGER_HPP

#include "Utils/AssetManager.hpp"
#include "Graphics/ShaderLoader.hpp"
#include "Graphics/TextureLoader.hpp"

namespace sd {

class GraphicsManager {
   public:
    static inline AssetManager &shaders() {
        static AssetManager s_instance;
        return s_instance;
    }

    static inline AssetManager &textures() {
        static AssetManager s_instance;
        return s_instance;
    }
};

}  // namespace sd

#endif /* SD_GRAPHICS_MANAGER_HPP */
