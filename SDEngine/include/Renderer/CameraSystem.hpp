#ifndef SD_CAMERA_SYSTEM_HPP
#define SD_CAMERA_SYSTEM_HPP

#include "Renderer/Export.hpp"
#include "Core/System.hpp"

namespace SD {

class SD_RENDERER_API CameraSystem : public System {
   public:
    CameraSystem();

    void OnRender() override;
};

}  // namespace SD

#endif
