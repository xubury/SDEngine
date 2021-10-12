#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include "Utility/Base.hpp"
#include "Core/ActionTarget.hpp"
#include "Graphics/Camera.hpp"

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class CameraController : public sd::ActionTarget<CameraMovement> {
   public:
    CameraController();

    void tick(float dt);

    void move(const glm::vec3 &t);

    void setCamera(sd::Camera *camera);

    void setFocus(const glm::vec3 &focus);

    void rotate(float yaw, float pitch);

    void rotateAround(float yaw, float pitch);

   private:
    sd::ActionMap<CameraMovement> m_controllerMap;
    sd::Camera *m_camera;
    glm::vec3 m_focus;

    glm::vec2 m_mouseMovement;
    glm::vec2 m_mouseSmoothMovement;
};

#endif /* CAMERA_CONTROLLER_HPP */
