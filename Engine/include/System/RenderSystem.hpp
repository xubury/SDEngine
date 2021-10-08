#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "Core/System.hpp"
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

    Framebuffer *getGBuffer() const { return m_gBuffer.get(); }
   private:
    void initQuad();
    void initGBuffer(int width, int height, int samples);

    void renderGBuffer();
    void renderShadow();
    void renderLight();

    void renderBlur();

    void renderMain();

    Ref<Shader> m_mainShader;

    Ref<Shader> m_blurShader;
    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    Ref<Shader> m_lightShader;
    RenderTarget m_lightTarget[2];
    Texture *m_lightResult;

    Ref<Shader> m_gBufferShader;
    RenderTarget m_gBufferTarget;
    Ref<Framebuffer> m_gBuffer;

    Ref<Shader> m_shadowShader;

    Ref<VertexArray> m_quad;

    Scene *m_scene;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
