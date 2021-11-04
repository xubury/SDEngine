#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "System/System.hpp"
#include "System/Event.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderTarget.hpp"

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

class SD_API RenderSystem : public System {
   public:
    RenderSystem(int width, int height, int samples);

    void onInit() override;

    void onDestroy() override;

    void onTick(float dt) override;

    void onRender() override;

    void onSizeEvent(const SizeEvent &event);
    void onSceneEvent(const SceneEvent &event);

    Framebuffer &getGBuffer() { return *m_gBufferTarget.getFramebuffer(); }

    RenderTarget &getGBufferTarget() { return m_gBufferTarget; }

   private:
    void initShaders();
    void initLighting(int width, int height, int samples);
    void initBloom(int width, int height);
    void initQuad();
    void initSkybox();
    void initGBuffer(int width, int height, int samples);

    void clear();

    void renderGBuffer();
    void renderShadow();
    void renderDeferred();

    void renderEmissive();

    void renderText();

    RenderTarget &getBlurReuslt() { return m_blurTarget[0]; };
    RenderTarget &getLightResult() { return m_lightTarget[0]; };

    void renderBlur();
    void renderMain();

    void renderSkybox();

    Ref<Shader> m_mainShader;

    Ref<Shader> m_blurShader;
    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    Ref<Shader> m_emssiveShader;

    Ref<Shader> m_deferredShader;
    RenderTarget m_lightTarget[2];

    RenderTarget m_gBufferTarget;
    Ref<Shader> m_gBufferShader;

    Ref<VertexArray> m_quad;

    Ref<Shader> m_skyboxShader;
    Ref<VertexArray> m_skybox;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
