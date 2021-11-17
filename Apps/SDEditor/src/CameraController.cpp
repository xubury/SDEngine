#include "CameraController.hpp"
#include "Input/Input.hpp"
#include "Utility/Log.hpp"

namespace SD {

const float ROTATION_SPEED = 0.1;
const float SMOOTHNESS = 10;

CameraController::CameraController()
    : ActionTarget<CameraMovement>(m_controllerMap),
      m_camera(nullptr),
      m_focus(0.f),
      m_mouseMovement(0),
      m_mouseSmoothMovement(0),
      m_pitch(0.f) {
    m_controllerMap.Map(
        CameraMovement::FORWARD,
        Action(Keycode::W, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controllerMap.Map(
        CameraMovement::BACKWARD,
        Action(Keycode::S, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controllerMap.Map(
        CameraMovement::LEFT,
        Action(Keycode::A, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controllerMap.Map(
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
    Bind(Action(Event::EventType::MOUSE_MOTION), [this](const Event &event) {
        if (Input::IsMouseDown(MouseButton::RIGHT)) {
            m_mouseMovement.x += event.mouseMotion.xrel;
            m_mouseMovement.y += event.mouseMotion.yrel;
        }
    });
}

void CameraController::Tick(float dt) {
    m_mouseSmoothMovement =
        glm::mix(m_mouseSmoothMovement, m_mouseMovement, dt * SMOOTHNESS);
    Rotate(-m_mouseSmoothMovement.x * ROTATION_SPEED,
           -m_mouseSmoothMovement.y * ROTATION_SPEED);
    m_mouseMovement.x = 0;
    m_mouseMovement.y = 0;
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
