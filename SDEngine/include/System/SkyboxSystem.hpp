#ifndef SD_SKYBOX_SYSTEM_HPP
#define SD_SKYBOX_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "ImGui/FileDialog.hpp"

namespace SD {

class SD_SYSTEM_API SkyboxSystem : public System {
   public:
    SkyboxSystem(Framebuffer *framebuffer);

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnRender() override;

   private:
    Framebuffer *m_framebuffer;
    Ref<Shader> m_skybox_shader;
    Ref<VertexArray> m_box_vao;
    Ref<Texture> m_skybox;
};

}  // namespace SD

#endif
