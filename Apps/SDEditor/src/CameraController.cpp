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
    m_controllerMap.map(
        CameraMovement::FORWARD,
        Action(Keycode::w, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controllerMap.map(
        CameraMovement::BACKWARD,
        Action(Keycode::s, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controllerMap.map(
        CameraMovement::LEFT,
        Action(Keycode::a, Action::Type::REAL_TIME | Action::Type::DOWN));
    m_controllerMap.map(
        CameraMovement::RIGHT,
        Action(Keycode::d, Action::Type::REAL_TIME | Action::Type::DOWN));

    bind(CameraMovement::FORWARD,
         [this](const Event &) { move(-m_camera->getWorldFront()); });
    bind(CameraMovement::BACKWARD,
         [this](const Event &) { move(m_camera->getWorldFront()); });
    bind(CameraMovement::LEFT,
         [this](const Event &) { move(-m_camera->getWorldRight()); });
    bind(CameraMovement::RIGHT,
         [this](const Event &) { move(m_camera->getWorldRight()); });
    bind(Action(Event::EventType::MOUSE_MOTION), [this](const Event &event) {
        if (Input::isMouseDown(MouseButton::RIGHT)) {
            m_mouseMovement.x += event.mouseMotion.xrel;
            m_mouseMovement.y += event.mouseMotion.yrel;
        }
    });
}

void CameraController::tick(float dt) {
    m_mouseSmoothMovement =
        glm::mix(m_mouseSmoothMovement, m_mouseMovement, dt * SMOOTHNESS);
    rotate(-m_mouseSmoothMovement.x * ROTATION_SPEED,
           -m_mouseSmoothMovement.y * ROTATION_SPEED);
    m_mouseMovement.x = 0;
    m_mouseMovement.y = 0;
}

void CameraController::move(const glm::vec3 &t) { m_camera->translateWorld(t); }

void CameraController::rotate(float yaw, float pitch) {
    yaw = glm::radians(yaw);
    pitch = glm::radians(pitch);

    glm::quat rotation = m_camera->getWorldRotation();
    m_pitch += pitch;
    if (std::abs(m_pitch) < glm::radians(89.f)) {
        rotation = glm::angleAxis(pitch, m_camera->getWorldRight()) * rotation;
    } else {
        m_pitch -= pitch;
    }
    rotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * rotation;
    m_camera->setWorldRotation(rotation);
}

void CameraController::rotateAround(float yaw, float pitch) {
    yaw = glm::radians(yaw);
    pitch = glm::radians(pitch);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, m_focus);
    m_pitch += pitch;
    if (std::abs(m_pitch) < glm::radians(89.f)) {
        transform = glm::rotate(transform, pitch, m_camera->getWorldRight());
    } else {
        m_pitch -= pitch;
    }
    transform = glm::rotate(transform, yaw, glm::vec3(0.f, 1.f, 0.f));
    transform = glm::translate(transform, -m_focus);
    m_camera->setWorldTransform(transform * m_camera->getWorldTransform());
}

void CameraController::setCamera(Camera *camera) {
    m_camera = camera;
    m_pitch = glm::pitch(m_camera->getWorldRotation());
}

void CameraController::setFocus(const glm::vec3 &focus) { m_focus = focus; }

}  // namespace SD
