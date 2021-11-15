#ifndef SD_POST_PROCESS_SYSTEM_HPP
#define SD_POST_PROCESS_SYSTEM_HPP

#include "Graphics/VertexArray.hpp"
#include "Core/System.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

class SD_API PostProcessSystem : public System {
   public:
    PostProcessSystem(RenderTarget *target, int width, int height);

    void onPush() override;

    void onPop() override;

    void onRender() override;

    void onSizeEvent(const WindowSizeEvent &event);

    void setExposure(float exposure);
    float getExposure();

    void setBloom(bool isBloom);
    bool getBloom();

    void setBloomFactor(float bloom);
    float getBloomFactor();

    float getGammaCorrection();
    void setGammaCorrection(float gamma);

   private:
    void renderBlur();
    void renderPost();

    Ref<Shader> m_postShader;

    Ref<Shader> m_blurShader;

    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    RenderTarget m_postTarget;

    Ref<VertexArray> m_quad;

    RenderTarget *m_target;

    bool m_isBloom;
    float m_bloom;
    float m_exposure;
    float m_gammaCorrection;
};

}  // namespace SD
#endif
