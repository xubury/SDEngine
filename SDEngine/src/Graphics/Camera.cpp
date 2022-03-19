#include "Graphics/Camera.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace SD {

Camera::Camera() : m_view_outdated(true), m_projection_outdated(true) {}

Camera::Camera(CameraType type, float fov, float width, float height,
               float near_z, float far_z)
    : m_type(type),
      m_position(0.f),
      m_rotation(1.f, 0.f, 0.f, 0.f),
      m_fov(fov),
      m_near_width(width),
      m_near_height(height),
      m_near_z(near_z),
      m_far_z(far_z),
      m_view_outdated(true),
      m_projection_outdated(true) {}

void Camera::Resize(float width, float height) {
    m_near_width = width;
    m_near_height = height;
    m_projection_outdated = true;
}

void Camera::TranslateLocal(const glm::vec3 &t) {
    m_position += GetWorldRotation() * t;
    m_view_outdated = true;
}

void Camera::TranslateWorld(const glm::vec3 &t) {
    m_position += t;
    m_view_outdated = true;
}

void Camera::RotateLocal(const glm::quat &r) {
    m_rotation = GetWorldRotation() * r;
    m_view_outdated = true;
}

void Camera::RotateWorld(const glm::quat &r) {
    m_rotation = r * GetWorldRotation();
    m_view_outdated = true;
}

glm::vec3 Camera::GetWorldRight() const {
    return m_rotation * glm::vec3(1, 0, 0);
}

glm::vec3 Camera::GetWorldUp() const { return m_rotation * glm::vec3(0, 1, 0); }

glm::vec3 Camera::GetWorldFront() const {
    return m_rotation * glm::vec3(0, 0, 1);
}

void Camera::SetWorldPosition(const glm::vec3 &position) {
    m_position = position;
    m_view_outdated = true;
}

void Camera::SetWorldRotation(const glm::quat &rotation) {
    m_rotation = rotation;
    m_view_outdated = true;
}

glm::vec3 Camera::GetWorldPosition() const { return m_position; }

glm::quat Camera::GetWorldRotation() const { return m_rotation; }

glm::mat4 Camera::GetWorldTransform() const {
    return glm::translate(glm::mat4(1.0f), m_position) *
           glm::toMat4(m_rotation);
}

void Camera::SetWorldTransform(const glm::mat4 &transform) {
    glm::vec3 scale;
    Decompose(transform, m_position, m_rotation, scale);
    m_view_outdated = true;
}

void Camera::UpdateProjection() {
    if (m_type == CameraType::Orthographic) {
        m_projection = glm::ortho(-m_near_width / 2.f, m_near_width / 2.f,
                                  -m_near_height / 2.f, m_near_height / 2.f,
                                  m_near_z, m_far_z);
    } else if (m_type == CameraType::Perspective) {
        m_projection = glm::perspective(m_fov, m_near_width / m_near_height,
                                        m_near_z, m_far_z);
    } else {
        SD_CORE_ASSERT(false, "Invalid camera type!");
    }
    m_projection_outdated = false;
}

glm::mat4 Camera::GetView() const { return m_view; }

glm::mat4 Camera::GetView() {
    if (ViewOutdated()) {
        UpdateView();
    }
    return m_view;
}

glm::mat4 Camera::GetProjection() const { return m_projection; }

glm::mat4 Camera::GetProjection() {
    if (ProjectionOutdated()) {
        UpdateProjection();
    }
    return m_projection;
}

glm::mat4 Camera::GetViewPorjection() { return GetProjection() * GetView(); }

glm::mat4 Camera::GetViewPorjection() const {
    return GetProjection() * GetView();
}

bool Camera::ViewOutdated() const { return m_view_outdated; }

bool Camera::ProjectionOutdated() const { return m_projection_outdated; }

void Camera::UpdateView() {
    m_position = GetWorldPosition();
    m_rotation = GetWorldRotation();

    m_view =
        glm::lookAt(m_position, m_position - GetWorldFront(), GetWorldUp());

    m_view_outdated = false;
}

glm::vec3 Camera::MapClipToWorld(const glm::vec2 &clip) const {
    glm::vec4 world(clip, 0, 1.0f);
    world = glm::inverse(GetViewPorjection()) * world;
    world /= world.w;
    return world;
}

glm::vec3 Camera::MapWorldToClip(const glm::vec3 &world) const {
    glm::vec4 clip = GetViewPorjection() * glm::vec4(world, 1.0f);
    clip /= clip.w;
    return clip;
}

Math::Ray Camera::ComputeCameraRay(const glm::vec2 &clip) const {
    glm::vec4 near(clip, m_near_z, 1.0f);
    glm::vec4 far(clip, m_far_z, 1.0f);
    glm::mat4 inv_pv = glm::inverse(GetViewPorjection());

    near = inv_pv * near;
    near /= near.w;

    far = inv_pv * far;
    far /= far.w;

    Math::Ray ray(near, far - near);
    return ray;
}

void Camera::SetFOV(float fov) {
    m_fov = fov;
    m_projection_outdated = true;
}
float Camera::GetFOV() const { return m_fov; }

void Camera::SetNearWidth(float width) {
    m_near_width = width;
    m_projection_outdated = true;
}
void Camera::SetNearHeight(float height) {
    m_near_height = height;
    m_projection_outdated = true;
}

float Camera::GetNearWidth() const {
    switch (m_type) {
        case CameraType::Perspective:
            return GetNearHeight() * m_near_width / m_near_height;
        default:
            return m_near_width;
    }
}

float Camera::GetNearHeight() const {
    switch (m_type) {
        case CameraType::Perspective:
            return std::tan(m_fov / 2.f) * m_near_z * 2.f;
        default:
            return m_near_height;
    }
}

float Camera::GetFarWidth() const {
    switch (m_type) {
        case CameraType::Perspective:
            return GetFarHeight() * m_near_width / m_near_height;
        default:
            return m_near_width;
    }
}

float Camera::GetFarHeight() const {
    switch (m_type) {
        case CameraType::Perspective:
            return std::tan(m_fov / 2.f) * m_far_z * 2.f;
        default:
            return m_near_height;
    }
}
void Camera::SetNearZ(float near_z) {
    m_near_z = near_z;
    m_projection_outdated = true;
}
float Camera::GetNearZ() const { return m_near_z; }

void Camera::SetFarZ(float far_z) {
    m_far_z = far_z;
    m_projection_outdated = true;
}
float Camera::GetFarZ() const { return m_far_z; }

void Camera::SetCameraType(CameraType type) {
    m_type = type;
    m_projection_outdated = true;
}

CameraType Camera::GetCameraType() const { return m_type; }

}  // namespace SD
