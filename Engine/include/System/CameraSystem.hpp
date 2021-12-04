#ifndef SD_CAMERA_SYSTEM_HPP
#define SD_CAMERA_SYSTEM_HPP

#include "Core/System.hpp"

namespace SD {

class CameraSystem : public System {
   public:
    CameraSystem();

    void OnRender() override;
};

}  // namespace SD

#endif
