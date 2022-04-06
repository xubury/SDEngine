#ifndef SD_DEFERRED_RENDERER_HPP
#define SD_DEFERRED_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/CascadeShadow.hpp"
#include "Graphics/PointShadow.hpp"

namespace SD {

enum class GeometryBufferType {
    Position = 0,
    Normal,
    Albedo,
    Ambient,
    Emissive,
    EntityId,
    GBufferCount
};

DataFormat SD_RENDERER_API GetTextureFormat(GeometryBufferType type);

class SD_RENDERER_API DeferredRenderer {
   public:
    DeferredRenderer(int width, int height, MultiSampleLevel msaa);
    ~DeferredRenderer();

    void Render();

    void ImGui();

    void SetRenderSize(int32_t width, int32_t height);

   private:
    void InitShaders();
    void InitSSAOBuffers();
    void InitSSAOKernel();
    void InitLightingBuffers();

    void RenderGBuffer();
    void BlitGeometryBuffers();

    void RenderSSAO();

    void RenderShadowMap(const Transform &transform);
    void RenderPointShadowMap(const Transform &transform);
    void RenderDeferred();
    void RenderEmissive();

    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;

    CascadeShadow m_cascade_shadow;
    Ref<Shader> m_cascade_shader;
    Ref<Shader> m_cascade_debug_shader;
    Ref<Framebuffer> m_cascade_debug_target;
    Ref<Texture> m_cascade_debug_buffer;
    int m_debug_layer{0};

    PointShadow m_point_shadow;
    Ref<Shader> m_point_shadow_shader;

    Ref<Shader> m_emssive_shader;

    Ref<Shader> m_deferred_shader;
    Ref<Framebuffer> m_lighting_target[2];
    Ref<Texture> m_lighting_buffers[2];
    Texture *m_lighting_result;

    Ref<Shader> m_gbuffer_shader;
    Ref<Framebuffer> m_geometry_target_msaa;
    std::array<Ref<Texture>, static_cast<int>(GeometryBufferType::GBufferCount)>
        m_gbuffer_msaa;
    Ref<Framebuffer> m_geometry_target;
    std::array<Ref<Texture>, static_cast<int>(GeometryBufferType::GBufferCount)>
        m_gbuffer;
    Ref<Renderbuffer> m_depth_buffer;

    Ref<Shader> m_ssao_shader;
    Ref<Texture> m_ssao_buffer;
    Ref<Shader> m_ssao_blur_shader;
    Ref<Texture> m_ssao_blur_buffer;

    bool m_ssao_state;
    float m_ssao_radius;
    float m_ssao_bias;
    int m_ssao_power;
    Ref<Texture> m_ssao_noise;
    std::vector<Vector3f> m_ssao_kernel;
};

}  // namespace SD

#endif /* SD_LIGHTING_SYSTEM_HPP */
