#include "CameraController.hpp"
#include "Core/InputManager.hpp"
#include "Common/Log.hpp"

CameraController::CameraController()
    : sd::ActionTarget<CameraMovement>(m_controllerMap),
      m_camera(nullptr),
      m_yaw(0),
      m_pitch(0) {
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
             if (sd::InputManager::isMouseDown(1)) {
                 rotate(-event.motion.xrel * 0.1f, -event.motion.yrel * 0.1f);
             }
         });
}

void CameraController::move(CameraMovement movement) {
    switch (movement) {
        case CameraMovement::FORWARD:
            m_transform.translateLocal(m_transform.getFront());
            break;
        case CameraMovement::BACKWARD:
            m_transform.translateLocal(-m_transform.getFront());
            break;
        case CameraMovement::LEFT:
            m_transform.translateLocal(-m_transform.getRight());
            break;
        case CameraMovement::RIGHT:
            m_transform.translateLocal(m_transform.getRight());
            break;
    }
    m_camera->updateView();
}

void CameraController::rotate(float yaw, float pitch) {
    m_yaw += yaw;
    m_pitch += pitch;
    if (m_pitch > 89.f) {
        m_pitch = 89.f;
    } else if (m_pitch < -89.f) {
        m_pitch = -89.f;
    }
    m_transform.setWorldRotation(glm::radians(glm::vec3(m_pitch, m_yaw, 0.f)));
    m_camera->updateView();
}

void CameraController::setCamera(sd::Camera *camera) {
    m_camera = camera;
    camera->setTransform(&m_transform);
}
