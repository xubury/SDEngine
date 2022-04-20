#ifndef SD_SKYBOX_RENDERER_HPP
#define SD_SKYBOX_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Camera.hpp"
#include "Resource/Resource.hpp"

namespace SD {

class SD_RENDERER_API SkyboxRenderer {
   public:
    static void Init(ShaderCache& shaders, const Texture* skybox);
    static void Render();
};

}  // namespace SD

#endif
