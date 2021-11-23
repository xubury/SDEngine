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

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

    void OnSizeEvent(const WindowSizeEvent &event);

    void SetExposure(float exposure);
    float GetExposure();

    void SetBloom(bool isBloom);
    bool GetBloom();

    void SetBloomFactor(float bloom);
    float GetBloomFactor();

    float GetGammaCorrection();
    void SetGammaCorrection(float gamma);

   private:
    void RenderBlur();
    void RenderPost();

    Ref<Shader> m_postShader;

    Ref<Shader> m_blurShader;

    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    RenderTarget m_postTarget;

    Ref<VertexArray> m_quad;

    RenderTarget *m_target;

    bool m_is_bloom;
    float m_bloom_factor;
    float m_exposure;
    float m_gamma_correction;
};

}  // namespace SD
#endif
