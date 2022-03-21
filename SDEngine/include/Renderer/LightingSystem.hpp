#ifndef SD_LIGHTING_SYSTEM_HPP
#define SD_LIGHTING_SYSTEM_HPP

#include "Renderer/Export.hpp"
#include "Renderer/Event.hpp"
#include "Renderer/RenderSystem.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

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

    Framebuffer *GetGBuffer() { return m_gbuffer.get(); }

    Texture *GetSSAO() { return m_ssao_blur_buffer->GetTexture(); }

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

    void RenderShadowMap(CascadeShadow &shadow, const Transform &transform);
    void RenderGBuffer();

    void RenderSSAO();

    void RenderDeferred();

    void RenderEmissive();

    Ref<Framebuffer> GetLightingBuffer() { return m_light_buffer[0]; };

    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;

    Ref<Shader> m_cascade_shader;
    Ref<Shader> m_cascade_debug_shader;
    Ref<Framebuffer> m_cascade_debug_fb;
    int m_debug_layer{0};

    Ref<Shader> m_emssive_shader;

    Ref<Shader> m_deferred_shader;
    Ref<Framebuffer> m_light_buffer[2];

    Ref<Shader> m_gbuffer_shader;
    Ref<Framebuffer> m_gbuffer;

    Ref<Shader> m_ssao_shader;
    Ref<Framebuffer> m_ssao_buffer;
    Ref<Shader> m_ssao_blur_shader;
    Ref<Framebuffer> m_ssao_blur_buffer;

    bool m_ssao_state;
    float m_ssao_radius;
    float m_ssao_bias;
    int m_ssao_power;
    Ref<Texture> m_ssao_noise;
    std::vector<glm::vec3> m_ssao_kernel;

    HandlerRegistration m_size_handler;
};

}  // namespace SD

#endif /* SD_LIGHTING_SYSTEM_HPP */
