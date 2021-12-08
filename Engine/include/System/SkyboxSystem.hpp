#ifndef SD_SKYBOX_SYSTEM_HPP
#define SD_SKYBOX_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace SD {

class SD_SYSTEM_API SkyboxSystem : public System {
   public:
    SkyboxSystem();

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

   private:
    Ref<Shader> m_skyboxShader;
    Ref<VertexArray> m_skybox;
};

}  // namespace SD

#endif
