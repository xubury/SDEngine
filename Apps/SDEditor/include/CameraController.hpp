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

    void setFocus(const glm::vec3 &focus);

    void rotate(float yaw, float pitch);

    void rotateAround(float yaw, float pitch);

   private:
    sd::ActionMap<CameraMovement> m_controllerMap;
    sd::Camera *m_camera;
    glm::vec3 m_focus;
    float m_pitch;
    float m_yaw;
};

#endif /* CAMERA_CONTROLLER_HPP */
