#ifndef SD_LIGHTING_SYSTEM_HPP
#define SD_LIGHTING_SYSTEM_HPP

#include "Renderer/Export.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/CascadeShadow.hpp"

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

class SD_RENDERER_API LightingSystem : public RenderSystem {
   public:
    LightingSystem(int width, int height, MultiSampleLevel msaa);

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnRender() override;

    void OnSizeEvent(const RenderSizeEvent &event);

    Framebuffer *GetGBuffer() { return m_geometry_target_msaa.get(); }

    void SetSSAOState(bool ssao_state) { m_ssao_state = ssao_state; }
    bool GetSSAOState() const { return m_ssao_state; }

    float GetSSAORadius() const { return m_ssao_radius; }
    void SetSSAORadius(float radius);

    float GetSSAOBias() const { return m_ssao_bias; }
    void SetSSAOBias(float bias);

    uint8_t GetSSAOPower() const { return m_ssao_power; }
    void SetSSAOPower(uint8_t power) { m_ssao_power = power; }

   private:
    void InitShaders();
    void InitSSAO();
    void InitSSAOKernel();
    void InitLighting();

    void RenderGBuffer();
    void BlitGeometryBuffers();

    void RenderSSAO();

    void RenderShadowMap(CascadeShadow &shadow, const Transform &transform);
    void RenderDeferred();
    void RenderEmissive();

    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;

    Ref<Shader> m_cascade_shader;
    Ref<Shader> m_cascade_debug_shader;
    Ref<Framebuffer> m_cascade_debug_target;
    Ref<Texture> m_cascade_debug_buffer;
    int m_debug_layer{0};

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

    HandlerRegistration m_size_handler;
};

}  // namespace SD

#endif /* SD_LIGHTING_SYSTEM_HPP */
