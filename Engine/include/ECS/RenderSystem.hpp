#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "ECS/System.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Renderer/RenderTarget.hpp"

namespace sd {

class RenderSystem : public System {
   public:
    RenderSystem();

    void onTick(float dt) override;

    void onRender() override;

    void setScene(Scene *scene);

    void setRenderTarget(RenderTarget *target);

    void setCamera(Camera *camera);

   private:
    Scene *m_scene;
    Ref<Shader> m_shader;
    RenderTarget *m_target;
    Camera *m_camera;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
