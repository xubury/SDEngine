#ifndef SD_POST_PROCESS_SYSTEM_HPP
#define SD_POST_PROCESS_SYSTEM_HPP

#include "Renderer/System/System.hpp"
#include "Renderer/System/Event.hpp"
#include "Graphics/Shader.hpp"
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
    void renderPost();
    void renderSkybox();

    Ref<Shader> m_postShader;

    Ref<Shader> m_blurShader;
    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;
};

}  // namespace sd
#endif
