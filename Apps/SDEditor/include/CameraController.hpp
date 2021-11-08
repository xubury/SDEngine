#ifndef SD_CAMERA_CONTROLLER_HPP
#define SD_CAMERA_CONTROLLER_HPP

#include "Utility/Base.hpp"
#include "Input/ActionTarget.hpp"
#include "Renderer/Camera.hpp"

namespace sd {

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class CameraController : public ActionTarget<CameraMovement> {
   public:
    CameraController();

    void tick(float dt);

    void move(const glm::vec3 &t);

    void setCamera(Camera *camera);

    void setFocus(const glm::vec3 &focus);

    void rotate(float yaw, float pitch);

    void rotateAround(float yaw, float pitch);

   private:
    ActionMap<CameraMovement> m_controllerMap;
    Camera *m_camera;
    glm::vec3 m_focus;

    glm::vec2 m_mouseMovement;
    glm::vec2 m_mouseSmoothMovement;

    float m_pitch;
};

}  // namespace sd

#endif /* SD_CAMERA_CONTROLLER_HPP */
