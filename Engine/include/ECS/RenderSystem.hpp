#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "ECS/System.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Renderer/RenderTarget.hpp"

namespace sd {

class RenderEngine;

class SD_API RenderSystem : public System {
   public:
    RenderSystem(RenderEngine *engine, int width, int height, int samples);

    void onTick(float dt) override;

    void onRender() override;

    void onSceneChange() override{};

    void resize(int width, int height);

    Framebuffer *getGBuffer();

   private:
    void initQuad();
    void initGBuffer(int width, int height, int samples);

    void renderGBuffer();
    void renderLight();
    void renderBlur();
    void renderMain();

    RenderEngine *m_engine;
    Ref<Shader> m_mainShader;

    Ref<Shader> m_blurShader;
    Ref<Framebuffer> m_blurBuffer[2];
    Texture *m_blurResult;

    Ref<Shader> m_lightShader;
    Ref<Framebuffer> m_lightBuffer;

    Ref<Shader> m_gBufferShader;
    RenderTarget m_gBufferTarget;
    Ref<Framebuffer> m_gBuffer;

    Ref<VertexArray> m_vao;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
