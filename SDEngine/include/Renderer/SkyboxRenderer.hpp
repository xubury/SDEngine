#ifndef SD_SKYBOX_RENDERER_HPP
#define SD_SKYBOX_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Camera.hpp"

namespace SD {

class SD_RENDERER_API SkyboxRenderer {
   public:
    SkyboxRenderer();

    void Render();

   private:
    Ref<Shader> m_skybox_shader;
    Ref<Texture> m_skybox;
};

}  // namespace SD

#endif
