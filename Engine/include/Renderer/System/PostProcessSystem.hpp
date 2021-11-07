#ifndef SD_POST_PROCESS_SYSTEM_HPP
#define SD_POST_PROCESS_SYSTEM_HPP

#include "System/System.hpp"
#include "System/Event.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderTarget.hpp"

namespace sd {

class SD_API PostProcessSystem : public System {
   public:
    PostProcessSystem(int width, int height);

    void onInit() override;

    void onDestroy() override;

    void onRender() override;

    void onSizeEvent(const SizeEvent &event);

   private:
    void renderBlur();
    void renderMain();
    void renderSkybox();

    Ref<Shader> m_skyboxShader;
    Ref<VertexArray> m_skybox;

    Ref<Shader> m_mainShader;

    Ref<Shader> m_blurShader;
    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;
};

}  // namespace sd
#endif
