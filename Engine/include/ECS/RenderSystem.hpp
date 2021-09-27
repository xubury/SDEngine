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
    void renderGBuffer();
    void renderMain();

    RenderEngine *m_engine;
    Ref<Shader> m_shader;

    Ref<Shader> m_gbufferShader;
    RenderTarget m_gBufferTarget;
    Ref<Framebuffer> m_framebuffer;

    Ref<VertexArray> m_vao;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
