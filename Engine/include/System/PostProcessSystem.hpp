#ifndef SD_POST_PROCESS_SYSTEM_HPP
#define SD_POST_PROCESS_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

class SD_SYSTEM_API PostProcessSystem : public System {
   public:
    PostProcessSystem(int width, int height);

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

    Ref<Shader> m_post_shader;

    Ref<Shader> m_blur_shader;

    RenderTarget m_blur_target[2];
    Texture *m_blur_result;

    RenderTarget m_post_target;

    Ref<VertexArray> m_quad;

    bool m_is_bloom;
    float m_bloom_factor;
    float m_exposure;
    float m_gamma_correction;

    HandlerRegistration m_size_handler;
};

}  // namespace SD
#endif
