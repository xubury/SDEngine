#ifndef SD_CAMERA_CONTROLLER_HPP
#define SD_CAMERA_CONTROLLER_HPP

#include "Utility/Base.hpp"
#include "Input/ActionTarget.hpp"
#include "Renderer/Camera.hpp"

namespace SD {

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class CameraController : public ActionTarget<CameraMovement> {
   public:
    CameraController();

    void Tick(float dt);

    void Move(const glm::vec3 &t);

    void SetCamera(Camera *camera);

    void SetFocus(const glm::vec3 &focus);

    void Rotate(float yaw, float pitch);

    void RotateAround(float yaw, float pitch);

   private:
    ActionMap<CameraMovement> m_controller_map;
    Camera *m_camera;
    glm::vec3 m_focus;

    glm::vec2 m_mouse_movement;
    glm::vec2 m_mouse_smooth_movement;

    float m_pitch;
};

}  // namespace SD

#endif /* SD_CAMERA_CONTROLLER_HPP */
