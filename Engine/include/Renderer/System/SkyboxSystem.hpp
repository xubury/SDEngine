
#ifndef SD_SKYBOX_SYSTEM_HPP
#define SD_SKYBOX_SYSTEM_HPP

#include "Renderer/System/System.hpp"
#include "Renderer/System/Event.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace sd {

class SD_API SkyboxSystem : public System {
   public:
    SkyboxSystem();

    void onInit() override;

    void onDestroy() override;

    void onRender() override;

   private:
    Ref<Shader> m_skyboxShader;
    Ref<VertexArray> m_skybox;
};

}  // namespace sd

#endif
