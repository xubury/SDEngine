#ifndef SD_LIGHTING_SYSTEM_HPP
#define SD_LIGHTING_SYSTEM_HPP

#include "System/Export.hpp"
#include "Core/System.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace SD {

enum GeometryBufferType {
    G_POSITION = 0,
    G_NORMAL,
    G_ALBEDO,
    G_AMBIENT,
    G_EMISSIVE,
    G_ENTITY_ID,
    G_DEPTH,
    GBUFFER_COUNT
};

TextureFormat SD_SYSTEM_API GetTextureFormat(GeometryBufferType type);

TextureFormatType SD_SYSTEM_API GetTextureFormatType(GeometryBufferType type);

class SD_SYSTEM_API LightingSystem : public System {
   public:
    LightingSystem(RenderTarget *target, int width, int height, int samples);

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

    void OnSizeEvent(const WindowSizeEvent &event);

    Framebuffer *GetGBuffer() { return m_gbuffer_target.GetFramebuffer(); }

    Texture *GetSSAO() { return m_ssao_target.GetTexture(); }

   private:
    void InitShaders();
    void InitLighting(int width, int height, int samples);

    void Clear();

    void RenderShadowMap();
    void RenderGBuffer();

    void RenderSSAO();

    void RenderDeferred();

    void RenderEmissive();

    RenderTarget &GetLightingTarget() { return m_light_target[0]; };
    Ref<Shader> m_shadow_shader;

    RenderTarget *m_target;

    Ref<Shader> m_emssive_shader;

    Ref<Shader> m_deferred_shader;
    RenderTarget m_light_target[2];

    Ref<Shader> m_gbuffer_shader;
    RenderTarget m_gbuffer_target;

    Ref<Shader> m_ssao_shader;
    RenderTarget m_ssao_target;

    Ref<Texture> m_ssao_noise;

    Ref<Shader> m_ssao_blur_shader;
    RenderTarget m_ssao_blur_target;

    std::vector<glm::vec3> m_ssao_kernel;

    Ref<VertexArray> m_quad;

    HandlerRegistration m_size_handler;
};

}  // namespace SD

#endif /* SD_LIGHTING_SYSTEM_HPP */
