#ifndef SD_POST_PROCESS_RENDERER_HPP
#define SD_POST_PROCESS_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Framebuffer.hpp"

namespace SD {

class SD_RENDERER_API PostProcessRenderer {
   public:
    PostProcessRenderer(int32_t width, int32_t height);
    ~PostProcessRenderer();

    void SetRenderSize(int32_t width, int32_t height);

    void ImGui();

    void Render();

   private:
    void RenderPost();

    void RenderBloom();
    void Downsample(Texture &src, Texture &dst);
    void Upsample(Texture &src, Texture &dst);

    void InitBuffers();

    int m_width;
    int m_height;

    Ref<Shader> m_hdr_shader;
    Ref<Shader> m_bloom_shader;

    Ref<Framebuffer> m_post_target;
    Ref<Texture> m_post_buffer;

    Ref<Texture> m_upsample_buffer;
    Ref<Texture> m_downsample_buffer;
    float m_bloom_threshold;
    float m_bloom_soft_threshold;

    bool m_is_bloom;
    float m_exposure;
    float m_gamma_correction;
};

}  // namespace SD
#endif
