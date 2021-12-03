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

   private:
    void InitShaders();
    void InitLighting(int width, int height, int samples);

    void Clear();

    void RenderGBuffer();
    void RenderShadow();
    void RenderDeferred();

    void RenderEmissive();

    RenderTarget &GetLightingTarget() { return m_light_target[0]; };

    RenderTarget *m_target;

    Ref<Shader> m_emssive_shader;

    Ref<Shader> m_deferred_shader;
    RenderTarget m_light_target[2];

    Ref<Shader> m_gbuffer_shader;
    RenderTarget m_gbuffer_target;

    Ref<VertexArray> m_quad;

    HandlerRegistration m_size_handler;
};

}  // namespace SD

#endif /* SD_LIGHTING_SYSTEM_HPP */
