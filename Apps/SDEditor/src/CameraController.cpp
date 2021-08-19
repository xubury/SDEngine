#include "CameraController.hpp"
#include "Core/InputManager.hpp"
#include "Common/Log.hpp"

CameraController::CameraController()
    : sd::ActionTarget<CameraMovement>(m_controllerMap),
      m_camera(nullptr),
      m_focus(0.f) {
    m_controllerMap.map(CameraMovement::FORWARD,
                        sd::Action(SDLK_s, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));
    m_controllerMap.map(CameraMovement::BACKWARD,
                        sd::Action(SDLK_w, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));
    m_controllerMap.map(CameraMovement::LEFT,
                        sd::Action(SDLK_a, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));
    m_controllerMap.map(CameraMovement::RIGHT,
                        sd::Action(SDLK_d, sd::Action::Type::REAL_TIME |
                                               sd::Action::Type::DOWN));

    bind(CameraMovement::FORWARD,
         [this](const SDL_Event &) { move(CameraMovement::FORWARD); });
    bind(CameraMovement::BACKWARD,
         [this](const SDL_Event &) { move(CameraMovement::BACKWARD); });
    bind(CameraMovement::LEFT,
         [this](const SDL_Event &) { move(CameraMovement::LEFT); });
    bind(CameraMovement::RIGHT,
         [this](const SDL_Event &) { move(CameraMovement::RIGHT); });
    bind(sd::Action(SDL_EventType::SDL_MOUSEMOTION),
         [this](const SDL_Event &event) {
             if (sd::InputManager::isMouseDown(SDL_BUTTON_RIGHT)) {
                 rotateAround(-event.motion.xrel * 0.1f,
                              -event.motion.yrel * 0.1f);
             } else if (sd::InputManager::isMouseDown(SDL_BUTTON_MIDDLE)) {
                 rotate(-event.motion.xrel * 0.1f, -event.motion.yrel * 0.1f);
             }
         });
}

void CameraController::move(CameraMovement movement) {
    switch (movement) {
        case CameraMovement::FORWARD:
            m_camera->translateLocal(m_camera->getLocalFront());
            break;
        case CameraMovement::BACKWARD:
            m_camera->translateLocal(-m_camera->getLocalFront());
            break;
        case CameraMovement::LEFT:
            m_camera->translateLocal(-m_camera->getLocalRight());
            break;
        case CameraMovement::RIGHT:
            m_camera->translateLocal(m_camera->getLocalRight());
            break;
    }
}

void CameraController::rotate(float yaw, float pitch) {
    glm::quat rotation(1, 0, 0, 0);
    rotation = glm::angleAxis(glm::radians(pitch), m_camera->getWorldRight());
    rotation = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0)) * rotation;
    m_camera->setWorldRotation(rotation * m_camera->getWorldRotation());
}

void CameraController::rotateAround(float yaw, float pitch) {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, m_focus);
    transform =
        glm::rotate(transform, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
    transform =
        glm::rotate(transform, glm::radians(pitch), m_camera->getWorldRight());
    transform = glm::translate(transform, -m_focus);
    m_camera->setWorldTransform(transform * m_camera->getWorldTransform());
}

void CameraController::setCamera(sd::Camera *camera) { m_camera = camera; }

void CameraController::setFocus(const glm::vec3 &focus) { m_focus = focus; }
