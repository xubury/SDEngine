#ifndef SD_CAMERA_SYSTEM_HPP
#define SD_CAMERA_SYSTEM_HPP

#include "Renderer/Export.hpp"
#include "Core/ECSSystem.hpp"

namespace SD {

class SD_RENDERER_API CameraSystem : public ECSSystem {
   public:
    CameraSystem();

    void OnRender() override;
};

}  // namespace SD

#endif
