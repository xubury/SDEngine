#include "EditorCameraSystem.hpp"
#include "Core/Input.hpp"
#include "ImGui/ImGuiWidget.hpp"

namespace SD {

const float ROTATION_SPEED = 0.1;
const float TRANSLATION_SPEED = 100.f;
const float SMOOTHNESS = 10;
const float MAX_FAR_Z = 1000;

EditorCameraSystem::EditorCameraSystem(int32_t width, int32_t height)
    : System("EditorCameraSystem"),
      m_width(width),
      m_height(height),
      m_pitch(0),
      m_mouse_smooth_movement(0),
      m_mouse_movement(0),
      m_camera(CameraType::Perspective, glm::radians(45.f), m_width, m_height,
               0.1f, MAX_FAR_Z) {
    m_camera.SetWorldPosition(glm::vec3(0, 0, 1));
}

void EditorCameraSystem::OnInit() { System::OnInit(); }

void EditorCameraSystem::OnPush() {
    m_key_handler =
        EventSystem::Get().Register(this, &EditorCameraSystem::OnMouseMotion);
    m_size_handler =
        EventSystem::Get().Register(this, &EditorCameraSystem::OnSizeEvent);
}

void EditorCameraSystem::OnPop() {
    EventSystem::Get().RemoveHandler(m_key_handler);
    EventSystem::Get().RemoveHandler(m_size_handler);
}

void EditorCameraSystem::OnMouseMotion(const MouseMotionEvent &event) {
    if (Input::IsMouseDown(MouseButton::Right)) {
        m_mouse_movement.x += event.x_rel;
        m_mouse_movement.y += event.y_rel;
    }
}

void EditorCameraSystem::OnSizeEvent(const ViewportSizeEvent &event) {
    m_camera.Resize(event.width, event.height);
}

void EditorCameraSystem::OnImGui() {
    ImGui::Begin("Editor Camera Sysetm");
    {
        CameraType type = m_camera.GetCameraType();
        if (ImGui::RadioButton("Perspective", reinterpret_cast<int *>(&type),
                               static_cast<int>(CameraType::Perspective))) {
            m_camera.SetCameraType(type);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Ortho", reinterpret_cast<int *>(&type),
                               static_cast<int>(CameraType::Orthographic))) {
            m_camera.SetCameraType(type);
        }
        float fov = m_camera.GetFOV();
        ImGui::Text("Field of view");
        if (ImGui::SliderAngle("##FOV", &fov, 1.0f, 89.f)) {
            m_camera.SetFOV(fov);
        }
        float near_z = m_camera.GetNearZ();
        float far_z = m_camera.GetFarZ();
        ImGui::Text("Near Z");
        if (ImGui::SliderFloat("##Near Z", &near_z, 1e-3, far_z)) {
            m_camera.SetNearZ(near_z);
        }
        ImGui::Text("Far Z");
        if (ImGui::SliderFloat("##Far Z", &far_z, near_z, MAX_FAR_Z)) {
            m_camera.SetFarZ(far_z);
        }
    }
    ImGui::End();
}

void EditorCameraSystem::OnTick(float dt) {
    float step = TRANSLATION_SPEED * dt;
    if (Input::IsKeyDown(Keycode::W) &&
        m_camera.GetCameraType() == CameraType::Perspective) {
        m_camera.TranslateWorld(-m_camera.GetWorldFront() * step);
    }
    if (Input::IsKeyDown(Keycode::S) &&
        m_camera.GetCameraType() == CameraType::Perspective) {
        m_camera.TranslateWorld(m_camera.GetWorldFront() * step);
    }
    if (Input::IsKeyDown(Keycode::A)) {
        m_camera.TranslateWorld(-m_camera.GetWorldRight() * step);
    }
    if (Input::IsKeyDown(Keycode::D)) {
        m_camera.TranslateWorld(m_camera.GetWorldRight() * step);
    }
    if (Input::IsKeyDown(Keycode::LShift)) {
        m_camera.TranslateWorld(m_camera.GetWorldUp() * step);
    }
    if (Input::IsKeyDown(Keycode::LCtrl)) {
        m_camera.TranslateWorld(-m_camera.GetWorldUp() * step);
    }
    m_mouse_smooth_movement =
        glm::mix(m_mouse_smooth_movement, m_mouse_movement, dt * SMOOTHNESS);
    if (m_allow_rotate) {
        Rotate(-m_mouse_smooth_movement.x * ROTATION_SPEED,
               -m_mouse_smooth_movement.y * ROTATION_SPEED);
    }
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

}  // namespace SD
