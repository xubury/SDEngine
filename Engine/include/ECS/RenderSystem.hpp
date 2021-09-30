#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "ECS/System.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderTarget.hpp"

namespace sd {

class RenderEngine;

class SD_API RenderSystem : public System {
   public:
    enum GBufferType {
        G_POSITION = 0,
        G_NORMAL,
        G_ALBEDO,
        G_AMBIENT,
        G_ENTITY_ID,
        GBUFFER_COUNT
    };

   public:
    RenderSystem(RenderEngine *engine, int width, int height, int samples);

    void onTick(float dt) override;

    void onRender() override;

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
    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    Ref<Shader> m_lightShader;
    RenderTarget m_lightTarget[2];
    Texture *m_lightResult;

    Ref<Shader> m_gBufferShader;
    RenderTarget m_gBufferTarget;
    Ref<Framebuffer> m_gBuffer;

    Ref<VertexArray> m_quad;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
