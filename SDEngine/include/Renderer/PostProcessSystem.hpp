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

    void OnDestroy() override;

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
    void RenderPost();

    void RenderBloom();
    void Downsample(Texture &src, Texture &dst);
    void Upsample(Texture &src, Texture &dst);

    void InitBuffers();

    int m_width;
    int m_height;

    Ref<Shader> m_post_shader;
    Ref<Shader> m_blur_shader;
    Ref<Shader> m_bloom_shader;

    Ref<Framebuffer> m_blur_targets[2];
    Ref<Texture> m_blur_buffers[2];
    Texture *m_blur_result;

    Ref<Framebuffer> m_post_target;
    Ref<Texture> m_post_buffer;

    Ref<Texture> m_upsample_buffer;
    Ref<Texture> m_downsample_buffer;
    float m_bloom_threshold;
    float m_bloom_soft_threshold;

    bool m_is_bloom;
    float m_exposure;
    float m_gamma_correction;

    HandlerRegistration m_size_handler;
};

}  // namespace SD
#endif
