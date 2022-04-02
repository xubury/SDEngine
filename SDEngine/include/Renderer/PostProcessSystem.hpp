#ifndef SD_POST_PROCESS_SYSTEM_HPP
#define SD_POST_PROCESS_SYSTEM_HPP

#include "Core/ECSSystem.hpp"
#include "Renderer/Export.hpp"
#include "Renderer/Event.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Framebuffer.hpp"

namespace SD {

class SD_RENDERER_API PostProcessSystem : public ECSSystem {
   public:
    PostProcessSystem(int32_t width, int32_t height);

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnRender() override;

    void OnSizeEvent(const RenderSizeEvent &event);

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

    void RenderMipmapTest();

    void InitBuffers();

    int m_width;
    int m_height;

    Ref<Shader> m_post_shader;
    Ref<Shader> m_blur_shader;
    Ref<Shader> m_test_shader;

    Ref<Framebuffer> m_blur_targets[2];
    Ref<Texture> m_blur_buffers[2];
    Texture *m_blur_result;

    Ref<Framebuffer> m_post_target;
    Ref<Texture> m_post_buffer;

    Ref<Framebuffer> m_test_target;
    Ref<Texture> m_test_buffer;

    bool m_is_bloom;
    float m_bloom_factor;
    float m_exposure;
    float m_gamma_correction;

    HandlerRegistration m_size_handler;
};

}  // namespace SD
#endif
