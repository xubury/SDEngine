#ifndef SD_CAMERA_SYSTEM_HPP
#define SD_CAMERA_SYSTEM_HPP

#include "Core/System.hpp"

namespace SD {

class CameraSystem : public System {
   public:
    CameraSystem();

    void OnTick(float dt) override;
};

}  // namespace SD

#endif
