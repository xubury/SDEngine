#ifndef SD_SKYBOX_RENDERER_HPP
#define SD_SKYBOX_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Camera.hpp"

namespace SD {

class SD_RENDERER_API SkyboxRenderer {
   public:
    static void Init();

    static void Render(const Camera& camera);

   private:
    static Ref<Shader> s_skybox_shader;
    static Ref<Texture> s_skybox;
};

}  // namespace SD

#endif
