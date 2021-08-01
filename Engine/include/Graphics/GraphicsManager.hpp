#ifndef SD_GRAPHICS_MANAGER_HPP
#define SD_GRAPHICS_MANAGER_HPP

#include "Utils/AssetManager.hpp"
#include "Graphics/ShaderLoader.hpp"
#include "Graphics/TextureLoader.hpp"

namespace sd {

class GraphicsManager {
   public:
    static GraphicsManager &instance() {
        static GraphicsManager s_instance;
        return s_instance;
    }
    ~GraphicsManager() = default;

    GraphicsManager(const GraphicsManager &) = delete;
    GraphicsManager &operator=(const GraphicsManager &) = delete;

    AssetManager &shaders() { return m_shaderManager; }

    AssetManager &textures() { return m_textureManager; }

   private:
    GraphicsManager() {
        m_shaderManager.setRootPath("assets/shaders/");
        m_shaderManager.setLoader<ShaderLoader>();

        m_textureManager.setRootPath("assets/textures/");
        m_textureManager.setLoader<TextureLoader>();
    };

    AssetManager m_shaderManager;
    AssetManager m_textureManager;
};
}  // namespace sd

#endif /* SD_GRAPHICS_MANAGER_HPP */
