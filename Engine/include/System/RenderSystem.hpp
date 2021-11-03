#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "System/System.hpp"
#include "System/Event.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderTarget.hpp"

namespace sd {


class SD_API RenderSystem : public System {
   public:
    RenderSystem(int width, int height, int samples);

    void onInit() override;

    void onDestroy() override;

    void onTick(float dt) override;

    void onRender() override;

    void onSizeEvent(const SizeEvent &event);
    void onSceneEvent(const SceneEvent &event);

    const RenderTarget &getLightResult() const { return m_lightTarget[0]; };

    Framebuffer &getGBuffer() { return *m_gBuffer; }

    RenderTarget &getGBufferTarget() { return m_gBufferTarget; }

   private:
    void initShaders();
    void initLighting(int width, int height);
    void initBloom(int width, int height);
    void initQuad();
    void initSkybox();
    void initGBuffer(int width, int height, int samples);

    void renderGBuffer();
    void renderShadow();
    void renderLight();

    void renderEmissive();

    void renderText();

    void renderBlur();
    void renderMain();

    void renderSkybox();

    Ref<Shader> m_mainShader;

    Ref<Shader> m_blurShader;
    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    Ref<Shader> m_emssiveShader;

    Ref<Shader> m_lightShader;
    RenderTarget m_lightTarget[2];

    RenderTarget m_gBufferTarget;
    Ref<Framebuffer> m_gBuffer;
    Ref<Shader> m_gBufferShader;

    Ref<VertexArray> m_quad;

    Ref<Shader> m_skyboxShader;
    Ref<VertexArray> m_skybox;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
