#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "ECS/System.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Renderer/RenderTarget.hpp"

namespace sd {

class SD_API RenderSystem : public System {
   public:
    RenderSystem();

    void onTick(float dt) override;

    void onRender() override;

    void onSceneChange() override{};

    void resize(int width, int height);

    void setRenderTarget(RenderTarget *target);

    void setCamera(Camera *camera);

    void renderGBuffer();

    Framebuffer *getGBuffer();

   private:
    Ref<Shader> m_shader;
    RenderTarget *m_target;
    Camera *m_camera;

    Ref<Shader> m_gbufferShader;
    RenderTarget m_gBufferTarget;
    Ref<Framebuffer> m_framebuffer;

    Ref<VertexArray> m_vao;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
