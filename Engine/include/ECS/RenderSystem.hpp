#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "ECS/System.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"

namespace sd {

class RenderSystem : public System {
   public:
    RenderSystem(Scene *scene);

    void onTick(float dt) override;

    void onRender() override;

    void setScene(Scene *scene);

   private:
    Scene *m_scene;
    Ref<Shader> m_shader;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
