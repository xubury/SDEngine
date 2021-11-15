
#ifndef SD_SKYBOX_SYSTEM_HPP
#define SD_SKYBOX_SYSTEM_HPP

#include "Core/System.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace SD {

class SD_API SkyboxSystem : public System {
   public:
    SkyboxSystem(RenderTarget *target);

    void onPush() override;

    void onPop() override;

    void onRender() override;

   private:
    Ref<Shader> m_skyboxShader;
    Ref<VertexArray> m_skybox;

    RenderTarget *m_target;
};

}  // namespace SD

#endif
