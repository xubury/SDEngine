#ifndef SD_SHADOW_SYSTEM_HPP
#define SD_SHADOW_SYSTEM_HPP

#include "Core/System.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

class SD_API ShadowSystem : public System {
   public:
    ShadowSystem();

    void OnPush() override;

    void OnPop() override;

    void OnRender() override;

   private:
    Ref<Shader> m_shadowShader;
};

}  // namespace SD

#endif
