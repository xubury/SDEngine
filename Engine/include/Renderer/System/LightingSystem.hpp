#ifndef SD_LIGHTING_SYSTEM_HPP
#define SD_LIGHTING_SYSTEM_HPP

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

TextureFormat SD_API GetTextureFormat(GeometryBufferType type);

TextureFormatType SD_API GetTextureFormatType(GeometryBufferType type);

class SD_API LightingSystem : public System {
   public:
    LightingSystem(RenderTarget *target, int width, int height, int samples);

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

    void OnSizeEvent(const WindowSizeEvent &event);

    Framebuffer *GetGBuffer() { return m_gBufferTarget.GetFramebuffer(); }

   private:
    void InitShaders();
    void InitLighting(int width, int height, int samples);

    void Clear();

    void RenderGBuffer();
    void RenderShadow();
    void RenderDeferred();

    void RenderEmissive();

    RenderTarget &GetLightingTarget() { return m_lightTarget[0]; };

    RenderTarget *m_target;

    Ref<Shader> m_emssiveShader;

    Ref<Shader> m_deferredShader;
    RenderTarget m_lightTarget[2];

    Ref<Shader> m_gBufferShader;
    RenderTarget m_gBufferTarget;

    Ref<VertexArray> m_quad;
};

}  // namespace SD

#endif /* SD_LIGHTING_SYSTEM_HPP */
