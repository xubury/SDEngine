#ifndef SD_SHADOW_SYSTEM_HPP
#define SD_SHADOW_SYSTEM_HPP

#include "Core/System.hpp"
#include "Graphics/Shader.hpp"

namespace SD {

class SD_API ShadowSystem : public System {
   public:
    ShadowSystem();

    void onInit() override;

    void onDestroy() override;

    void onRender() override;

   private:
    Ref<Shader> m_shadowShader;
};

}  // namespace SD

#endif
