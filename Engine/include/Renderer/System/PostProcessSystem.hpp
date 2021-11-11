#ifndef SD_POST_PROCESS_SYSTEM_HPP
#define SD_POST_PROCESS_SYSTEM_HPP

#include "Graphics/VertexArray.hpp"
#include "Renderer/System/System.hpp"
#include "Renderer/System/Event.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

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

    Ref<Shader> m_postShader;

    Ref<Shader> m_blurShader;

    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    RenderTarget m_target;

    Ref<VertexArray> m_quad;
};

}  // namespace SD
#endif
