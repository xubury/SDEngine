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
    SkyboxSystem();

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnRender() override;

   private:
    Ref<Shader> m_skyboxShader;
    Ref<VertexArray> m_box_vao;
    ResourceId m_skybox_id;

    bool m_file_dialog_open{false};
    ImFileDialogInfo m_file_dialog_info;
};

}  // namespace SD

#endif
