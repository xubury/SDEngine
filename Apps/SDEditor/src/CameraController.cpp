#include "CameraController.hpp"
#include "Input/Input.hpp"
#include "Utility/Log.hpp"

namespace SD {

const float ROTATION_SPEED = 0.1;
const float SMOOTHNESS = 10;

CameraController::CameraController()
    : ActionTarget<CameraMovement>(m_controller_map),
      m_camera(nullptr),
      m_focus(0.f),
      m_mouse_movement(0),
      m_mouse_smooth_movement(0),
      m_pitch(0.f) {
    m_controller_map.Map(
        CameraMovement::FORWARD,
        Action(Keycode::W, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controller_map.Map(
        CameraMovement::BACKWARD,
        Action(Keycode::S, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controller_map.Map(
        CameraMovement::LEFT,
        Action(Keycode::A, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controller_map.Map(
        CameraMovement::RIGHT,
        Action(Keycode::D, Action::Type::REAL_TIME | Action::Type::DOWN));

    Bind(CameraMovement::FORWARD,
         [this](const Event &) { Move(-m_camera->GetWorldFront()); });
    Bind(CameraMovement::BACKWARD,
         [this](const Event &) { Move(m_camera->GetWorldFront()); });
    Bind(CameraMovement::LEFT,
         [this](const Event &) { Move(-m_camera->GetWorldRight()); });
    Bind(CameraMovement::RIGHT,
         [this](const Event &) { Move(m_camera->GetWorldRight()); });
    Bind(Action(EventType::MOUSE_MOTION), [this](const Event &event) {
        if (Input::IsMouseDown(MouseButton::RIGHT)) {
            m_mouse_movement.x += event.mouse_motion.x_rel;
            m_mouse_movement.y += event.mouse_motion.y_rel;
        }
    });
}

void CameraController::Tick(float dt) {
    m_mouse_smooth_movement =
        glm::mix(m_mouse_smooth_movement, m_mouse_movement, dt * SMOOTHNESS);
    Rotate(-m_mouse_smooth_movement.x * ROTATION_SPEED,
           -m_mouse_smooth_movement.y * ROTATION_SPEED);
    m_mouse_movement.x = 0;
    m_mouse_movement.y = 0;
}

void CameraController::Move(const glm::vec3 &t) { m_camera->TranslateWorld(t); }

void CameraController::Rotate(float yaw, float pitch) {
    yaw = glm::radians(yaw);
    pitch = glm::radians(pitch);

    glm::quat rotation = m_camera->GetWorldRotation();
    m_pitch += pitch;
    if (std::abs(m_pitch) < glm::radians(89.f)) {
        rotation = glm::angleAxis(pitch, m_camera->GetWorldRight()) * rotation;
    } else {
        m_pitch -= pitch;
    }
    rotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * rotation;
    m_camera->SetWorldRotation(rotation);
}

void CameraController::RotateAround(float yaw, float pitch) {
    yaw = glm::radians(yaw);
    pitch = glm::radians(pitch);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, m_focus);
    m_pitch += pitch;
    if (std::abs(m_pitch) < glm::radians(89.f)) {
        transform = glm::rotate(transform, pitch, m_camera->GetWorldRight());
    } else {
        m_pitch -= pitch;
    }
    transform = glm::rotate(transform, yaw, glm::vec3(0.f, 1.f, 0.f));
    transform = glm::translate(transform, -m_focus);
    m_camera->SetWorldTransform(transform * m_camera->GetWorldTransform());
}

void CameraController::SetCamera(Camera *camera) {
    m_camera = camera;
    m_pitch = glm::pitch(m_camera->GetWorldRotation());
}

void CameraController::SetFocus(const glm::vec3 &focus) { m_focus = focus; }

}  // namespace SD
