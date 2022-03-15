#ifndef SD_POST_PROCESS_SYSTEM_HPP
#define SD_POST_PROCESS_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

class SD_SYSTEM_API PostProcessSystem : public System {
   public:
    PostProcessSystem(Framebuffer *framebuffer);

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnRender() override;

    void OnSizeEvent(const ViewportSizeEvent &event);

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

    void InitBuffers();

    int m_width;
    int m_height;
    Framebuffer *m_framebuffer;

    Ref<Shader> m_post_shader;
    Ref<Shader> m_blur_shader;

    Ref<Framebuffer> m_blur_buffer[2];
    Texture *m_blur_result;

    Ref<Framebuffer> m_post_buffer;

    Ref<VertexArray> m_quad;

    bool m_is_bloom;
    float m_bloom_factor;
    float m_exposure;
    float m_gamma_correction;

    HandlerRegistration m_size_handler;
};

}  // namespace SD
#endif
