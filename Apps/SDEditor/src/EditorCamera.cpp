#include "EditorCamera.hpp"
#include "Core/Input.hpp"
#include "ImGui/ImGuiWidget.hpp"

namespace SD {

const float ROTATION_SPEED = 0.1;
const float TRANSLATION_SPEED = 100.f;
const float SMOOTHNESS = 10;
const float MAX_FAR_Z = 1000;

EditorCamera::EditorCamera(int32_t width, int32_t height)
    : Camera(CameraType::Perspective, glm::radians(45.f), width, height, 0.1f,
             MAX_FAR_Z),
      m_width(width),
      m_height(height),
      m_pitch(0),
      m_mouse_smooth_movement(0),
      m_mouse_movement(0)
{
}

void EditorCamera::Move(int32_t x, int32_t y)
{
    if (Input::IsMouseDown(MouseButton::Right)) {
        m_mouse_movement.x += x;
        m_mouse_movement.y += y;
    }
}

void EditorCamera::ImGui()
{
    ImGui::Begin("Editor Camera");
    {
        using underlying = std::underlying_type<CameraType>::type;
        CameraType type = GetCameraType();
        if (ImGui::RadioButton(
                "Perspective", reinterpret_cast<underlying *>(&type),
                static_cast<underlying>(CameraType::Perspective))) {
            SetCameraType(type);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton(
                "Ortho", reinterpret_cast<underlying *>(&type),
                static_cast<underlying>(CameraType::Orthographic))) {
            SetCameraType(type);
        }
        float fov = GetFOV();
        ImGui::Text("Field of view");
        if (ImGui::SliderAngle("##FOV", &fov, 1.0f, 89.f)) {
            SetFOV(fov);
        }
        float near_z = GetNearZ();
        float far_z = GetFarZ();
        ImGui::Text("Near Z");
        if (ImGui::SliderFloat("##Near Z", &near_z, 1e-3, far_z)) {
            SetNearZ(near_z);
        }
        ImGui::Text("Far Z");
        if (ImGui::SliderFloat("##Far Z", &far_z, near_z, MAX_FAR_Z)) {
            SetFarZ(far_z);
        }
    }
    ImGui::End();
}

void EditorCamera::Tick(float dt)
{
    float step = TRANSLATION_SPEED * dt;
    if (Input::IsKeyDown(Keycode::W) &&
        GetCameraType() == CameraType::Perspective) {
        TranslateWorld(-GetWorldFront() * step);
    }
    if (Input::IsKeyDown(Keycode::S) &&
        GetCameraType() == CameraType::Perspective) {
        TranslateWorld(GetWorldFront() * step);
    }
    if (Input::IsKeyDown(Keycode::A)) {
        TranslateWorld(-GetWorldRight() * step);
    }
    if (Input::IsKeyDown(Keycode::D)) {
        TranslateWorld(GetWorldRight() * step);
    }
    if (Input::IsKeyDown(Keycode::LShift)) {
        TranslateWorld(GetWorldUp() * step);
    }
    if (Input::IsKeyDown(Keycode::LCtrl)) {
        TranslateWorld(-GetWorldUp() * step);
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

void EditorCamera::Rotate(float yaw, float pitch)
{
    yaw = glm::radians(yaw);
    pitch = glm::radians(pitch);

    Quaternion rotation = GetWorldRotation();
    m_pitch += pitch;
    if (std::abs(m_pitch) < glm::radians(89.f)) {
        rotation = glm::angleAxis(pitch, GetWorldRight()) * rotation;
    }
    else {
        m_pitch -= pitch;
    }
    rotation = glm::angleAxis(yaw, Vector3f(0, 1, 0)) * rotation;
    SetWorldRotation(rotation);
}

}  // namespace SD
