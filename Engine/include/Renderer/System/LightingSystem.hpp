#ifndef SD_LIGHTING_SYSTEM_HPP
#define SD_LIGHTING_SYSTEM_HPP

#include "Renderer/System/System.hpp"
#include "Renderer/System/Event.hpp"
#include "Renderer/RenderTarget.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace sd {

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

TextureFormat SD_API getTextureFormat(GeometryBufferType type);

TextureFormatType SD_API getTextureFormatType(GeometryBufferType type);

class SD_API LightingSystem : public System {
   public:
    LightingSystem(int width, int height, int samples);

    void onInit() override;

    void onDestroy() override;

    void onRender() override;

    void onSizeEvent(const SizeEvent &event);

    Framebuffer *getGBuffer() { return m_gBufferTarget.getFramebuffer(); }

   private:
    void initShaders();
    void initLighting(int width, int height, int samples);

    void clear();

    void renderGBuffer();
    void renderShadow();
    void renderDeferred();

    void renderEmissive();

    RenderTarget &getLightingTarget() { return m_lightTarget[0]; };

    Ref<Shader> m_emssiveShader;

    Ref<Shader> m_deferredShader;
    RenderTarget m_lightTarget[2];

    Ref<Shader> m_gBufferShader;
    RenderTarget m_gBufferTarget;

    Ref<VertexArray> m_quad;
};

}  // namespace sd

#endif /* SD_LIGHTING_SYSTEM_HPP */
