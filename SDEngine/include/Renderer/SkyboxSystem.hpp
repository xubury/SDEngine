#ifndef SD_SKYBOX_SYSTEM_HPP
#define SD_SKYBOX_SYSTEM_HPP

#include "Renderer/Export.hpp"
#include "Core/System.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "ImGui/FileDialog.hpp"

namespace SD {

class SD_RENDERER_API SkyboxSystem : public System {
   public:
    SkyboxSystem();

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnRender() override;

   private:
    Ref<Shader> m_skybox_shader;
    Ref<Texture> m_skybox;
};

}  // namespace SD

#endif
