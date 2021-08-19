#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include "Core/ActionTarget.hpp"
#include "Graphics/Camera.hpp"

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class CameraController : public sd::ActionTarget<CameraMovement> {
   public:
    CameraController();

    void move(CameraMovement movement);

    void setCamera(sd::Camera *camera);

    void rotate(float yaw, float pitch);

   private:
    sd::ActionMap<CameraMovement> m_controllerMap;
    sd::Camera *m_camera;
    float m_yaw;
    float m_pitch;
};

#endif /* CAMERA_CONTROLLER_HPP */
