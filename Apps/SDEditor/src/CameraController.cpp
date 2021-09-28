#include "CameraController.hpp"
#include "Core/Input.hpp"
#include "Utils/Log.hpp"

const float ROTATION_SPEED = 0.1;
const float TRANSLATION_SPEED = 0.01;
const float SMOOTHNESS = 10;

CameraController::CameraController()
    : sd::ActionTarget<CameraMovement>(m_controllerMap),
      m_camera(nullptr),
      m_focus(0.f),
      m_pitch(0),
      m_yaw(0),
      m_mouseMovement(0),
      m_mouseSmoothMovement(0) {
    m_controllerMap.map(CameraMovement::FORWARD,
                        sd::Action(SDLK_w, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));
    m_controllerMap.map(CameraMovement::BACKWARD,
                        sd::Action(SDLK_s, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));
    m_controllerMap.map(CameraMovement::LEFT,
                        sd::Action(SDLK_a, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));
    m_controllerMap.map(CameraMovement::RIGHT,
                        sd::Action(SDLK_d, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));

    bind(CameraMovement::FORWARD,
         [this](const SDL_Event &) { move(-m_camera->getWorldFront()); });
    bind(CameraMovement::BACKWARD,
         [this](const SDL_Event &) { move(m_camera->getWorldFront()); });
    bind(CameraMovement::LEFT,
         [this](const SDL_Event &) { move(-m_camera->getWorldRight()); });
    bind(CameraMovement::RIGHT,
         [this](const SDL_Event &) { move(m_camera->getWorldRight()); });
    bind(sd::Action(SDL_EventType::SDL_MOUSEMOTION),
         [this](const SDL_Event &event) {
             if (sd::Input::isMouseDown(SDL_BUTTON_RIGHT)) {
                 m_mouseMovement.x += event.motion.xrel;
                 m_mouseMovement.y += event.motion.yrel;
             }
         });
}

void CameraController::tick(float dt) {
    m_mouseSmoothMovement =
        glm::mix(m_mouseSmoothMovement, m_mouseMovement, dt * SMOOTHNESS);
    if (sd::Input::isKeyDown(SDLK_LALT)) {
        rotate(-m_mouseSmoothMovement.x * ROTATION_SPEED,
               -m_mouseSmoothMovement.y * ROTATION_SPEED);
    } else if (sd::Input::isKeyDown(SDLK_LSHIFT)) {
        move(m_camera->getWorldRight() * -m_mouseSmoothMovement.x *
             TRANSLATION_SPEED);
        move(m_camera->getWorldUp() * m_mouseSmoothMovement.y *
             TRANSLATION_SPEED);
    } else {
        rotateAround(-m_mouseSmoothMovement.x * ROTATION_SPEED,
                     -m_mouseSmoothMovement.y * ROTATION_SPEED);
    }
    m_mouseMovement.x = 0;
    m_mouseMovement.y = 0;
}

void CameraController::move(const glm::vec3 &t) { m_camera->translateWorld(t); }

void CameraController::rotate(float yaw, float pitch) {
    glm::quat rotation = m_camera->getWorldRotation();
    m_yaw += yaw;
    m_pitch += pitch;
    if (m_pitch <= -89.f || m_pitch >= 89.f) {
        m_pitch -= pitch;
    } else {
        rotation =
            glm::angleAxis(glm::radians(pitch), m_camera->getWorldRight()) *
            rotation;
    }
    rotation = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0)) * rotation;
    m_camera->setWorldRotation(rotation);
}

void CameraController::rotateAround(float yaw, float pitch) {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, m_focus);
    m_yaw += yaw;
    m_pitch += pitch;
    if (m_pitch <= -89.f || m_pitch >= 89.f) {
        m_pitch -= pitch;
    } else {
        transform = glm::rotate(transform, glm::radians(pitch),
                                m_camera->getWorldRight());
    }
    transform =
        glm::rotate(transform, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
    transform = glm::translate(transform, -m_focus);
    m_camera->setWorldTransform(transform * m_camera->getWorldTransform());
}

void CameraController::setCamera(sd::Camera *camera) { m_camera = camera; }

void CameraController::setFocus(const glm::vec3 &focus) { m_focus = focus; }
