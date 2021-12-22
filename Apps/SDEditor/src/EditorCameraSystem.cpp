#include "EditorCameraSystem.hpp"
#include "Core/Input.hpp"

namespace SD {

const float ROTATION_SPEED = 0.1;
const float SMOOTHNESS = 10;

EditorCameraSystem::EditorCameraSystem(uint32_t width, uint32_t height)
    : System("EditorCameraSystem"),
      m_width(width),
      m_height(height),
      m_pitch(0),
      m_mouse_smooth_movement(0),
      m_mouse_movement(0),
      m_camera(CameraType::PERSPECTIVE, glm::radians(45.f), m_width, m_height,
               0.1f, 1000.f) {
    m_camera.SetWorldPosition(glm::vec3(0, 0, 1));
}

void EditorCameraSystem::OnInit() { ActiveEditorCam(true); }

void EditorCameraSystem::OnPush() {
    m_key_handler =
        dispatcher->Register(this, &EditorCameraSystem::OnMouseMotion);
    m_size_handler =
        dispatcher->Register(this, &EditorCameraSystem::OnSizeEvent);
}

void EditorCameraSystem::OnPop() {
    dispatcher->RemoveHandler(m_key_handler);
    dispatcher->RemoveHandler(m_size_handler);
}

void EditorCameraSystem::OnMouseMotion(const MouseMotionEvent &event) {
    if (Input::IsMouseDown(MouseButton::RIGHT)) {
        m_mouse_movement.x += event.x_rel;
        m_mouse_movement.y += event.y_rel;
    }
}

void EditorCameraSystem::OnSizeEvent(const WindowSizeEvent &event) {
    m_camera.Resize(event.width, event.height);
}

void EditorCameraSystem::OnTick(float dt) {
    if (Input::IsKeyDown(Keycode::W)) {
        if (m_camera.GetCameraType() == CameraType::ORTHOGRAPHIC) {
            m_camera.TranslateWorld(m_camera.GetWorldUp());
        } else {
            m_camera.TranslateWorld(-m_camera.GetWorldFront());
        }
    }
    if (Input::IsKeyDown(Keycode::S)) {
        if (m_camera.GetCameraType() == CameraType::ORTHOGRAPHIC) {
            m_camera.TranslateWorld(-m_camera.GetWorldUp());
        } else {
            m_camera.TranslateWorld(m_camera.GetWorldFront());
        }
    }
    if (Input::IsKeyDown(Keycode::A)) {
        m_camera.TranslateWorld(-m_camera.GetWorldRight());
    }
    if (Input::IsKeyDown(Keycode::D)) {
        m_camera.TranslateWorld(m_camera.GetWorldRight());
    }
    m_mouse_smooth_movement =
        glm::mix(m_mouse_smooth_movement, m_mouse_movement, dt * SMOOTHNESS);
    Rotate(-m_mouse_smooth_movement.x * ROTATION_SPEED,
           -m_mouse_smooth_movement.y * ROTATION_SPEED);
    m_mouse_movement.x = 0;
    m_mouse_movement.y = 0;
}

void EditorCameraSystem::Rotate(float yaw, float pitch) {
    yaw = glm::radians(yaw);
    pitch = glm::radians(pitch);

    glm::quat rotation = m_camera.GetWorldRotation();
    m_pitch += pitch;
    if (std::abs(m_pitch) < glm::radians(89.f)) {
        rotation = glm::angleAxis(pitch, m_camera.GetWorldRight()) * rotation;
    } else {
        m_pitch -= pitch;
    }
    rotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * rotation;
    m_camera.SetWorldRotation(rotation);
}

void EditorCameraSystem::ActiveEditorCam(bool active) {
    if (active) {
        scene->SetCamera(&m_camera);
    } else {
        auto view = scene->view<CameraComponent>();
        auto iter = view.begin();
        if (iter != view.end()) {
            auto &camComp = view.get<CameraComponent>(*iter);
            scene->SetCamera(&camComp.camera);
        }
    }
}

}  // namespace SD
