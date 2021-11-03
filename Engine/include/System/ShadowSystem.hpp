#ifndef SD_SHADOW_SYSTEM_HPP
#define SD_SHADOW_SYSTEM_HPP

#include "System/System.hpp"
#include "Graphics/Shader.hpp"

namespace sd {

class SD_API ShadowSystem : public System {
   public:
    ShadowSystem();

    void onTick(float dt) override;

    void onRender() override;
   private:
    Ref<Shader> m_shadowShader;
};

}  // namespace sd

#endif
