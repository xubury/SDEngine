#include "Renderer/Camera.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace SD {

Camera::Camera(float zNear, float zFar)
    : m_zNear(zNear),
      m_zFar(zFar),
      m_transform(nullptr),
      m_position(0.f),
      m_rotation(1.f, 0.f, 0.f, 0.f),
      m_outdated(true) {
    UpdateView();
}

Camera::Camera(Transform *transform, float zNear, float zFar)
    : m_zNear(zNear),
      m_zFar(zFar),
      m_transform(transform),
      m_position(0.f),
      m_rotation(1.f, 0.f, 0.f, 0.f),
      m_outdated(true) {
    UpdateView();
}

Camera::Camera(const glm::vec3 &position, const glm::quat &rotation,
               float zNear, float zFar)
    : m_zNear(zNear),
      m_zFar(zFar),
      m_transform(nullptr),
      m_position(position),
      m_rotation(rotation),
      m_outdated(true) {
    UpdateView();
}

void Camera::TranslateLocal(const glm::vec3 &t) {
    if (m_transform) {
        m_transform->TranslateLocal(t);
    } else {
        m_position += GetWorldRotation() * t;
    }
    m_outdated = true;
}

void Camera::TranslateWorld(const glm::vec3 &t) {
    if (m_transform) {
        m_transform->TranslateWorld(t);
    } else {
        m_position += t;
    }
    m_outdated = true;
}

void Camera::RotateLocal(const glm::quat &r) {
    if (m_transform) {
        m_transform->RotateLocal(r);
    } else {
        m_rotation = GetWorldRotation() * r;
    }
    m_outdated = true;
}

void Camera::RotateWorld(const glm::quat &r) {
    if (m_transform) {
        m_transform->RotateWorld(r);
    } else {
        m_rotation = r * GetWorldRotation();
    }
    m_outdated = true;
}

glm::vec3 Camera::GetLocalRight() const {
    return m_transform ? m_transform->GetLocalRight()
                       : m_rotation * glm::vec3(1, 0, 0);
}

glm::vec3 Camera::GetLocalUp() const {
    return m_transform ? m_transform->GetLocalUp()
                       : m_rotation * glm::vec3(0, 1, 0);
}

glm::vec3 Camera::GetLocalFront() const {
    return m_transform ? m_transform->GetLocalFront()
                       : m_rotation * glm::vec3(0, 0, 1);
}

glm::vec3 Camera::GetWorldRight() const {
    return m_transform ? m_transform->GetWorldRight()
                       : m_rotation * glm::vec3(1, 0, 0);
}

glm::vec3 Camera::GetWorldUp() const {
    return m_transform ? m_transform->GetWorldUp()
                       : m_rotation * glm::vec3(0, 1, 0);
}

glm::vec3 Camera::GetWorldFront() const {
    return m_transform ? m_transform->GetWorldFront()
                       : m_rotation * glm::vec3(0, 0, 1);
}

void Camera::SetTransform(Transform *transform) { m_transform = transform; }

void Camera::SetLocalPosition(const glm::vec3 &position) {
    if (m_transform) {
        m_transform->SetLocalPosition(position);
    } else {
        m_position = position;
    }
    m_outdated = true;
}

void Camera::SetLocalRotation(const glm::quat &rotation) {
    if (m_transform) {
        m_transform->SetLocalRotation(rotation);
    } else {
        m_rotation = rotation;
    }
    m_outdated = true;
}

void Camera::SetWorldPosition(const glm::vec3 &position) {
    if (m_transform) {
        m_transform->SetWorldPosition(position);
    } else {
        m_position = position;
    }
    m_outdated = true;
}

void Camera::SetWorldRotation(const glm::quat &rotation) {
    if (m_transform) {
        m_transform->SetWorldRotation(rotation);
    } else {
        m_rotation = rotation;
    }
    m_outdated = true;
}

glm::vec3 Camera::GetLocalPosition() const {
    return m_transform ? m_transform->GetLocalPosition() : m_position;
}

glm::vec3 Camera::GetWorldPosition() const {
    return m_transform ? m_transform->GetWorldPosition() : m_position;
}

glm::quat Camera::GetLocalRotation() const {
    return m_transform ? m_transform->GetLocalRotation() : m_rotation;
}

glm::quat Camera::GetWorldRotation() const {
    return m_transform ? m_transform->GetWorldRotation() : m_rotation;
}

glm::mat4 Camera::GetLocalTransform() const {
    if (m_transform) {
        return m_transform->GetLocalTransform();
    } else {
        return glm::translate(glm::mat4(1.0f), m_position) *
               glm::toMat4(m_rotation);
    }
}

glm::mat4 Camera::GetWorldTransform() const {
    if (m_transform) {
        return m_transform->GetWorldTransform();
    } else {
        return glm::translate(glm::mat4(1.0f), m_position) *
               glm::toMat4(m_rotation);
    }
}

void Camera::SetWorldTransform(const glm::mat4 &transform) {
    if (m_transform) {
        m_transform->SetWorldTransform(transform);
    } else {
        glm::vec3 scale;
        Decompose(transform, m_position, m_rotation, scale);
    }
    m_outdated = true;
}

void Camera::SetLocalTransform(const glm::mat4 &transform) {
    if (m_transform) {
        m_transform->SetLocalTransform(transform);
    } else {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::vec3 scale;
        glm::decompose(transform, scale, m_rotation, m_position, skew,
                       perspective);
    }
    m_outdated = true;
}

void Camera::SetProjection(const glm::mat4 &projection) {
    m_projection = projection;
}

glm::mat4 Camera::GetView() const { return m_view; }

glm::mat4 Camera::GetView() {
    if (Outdated()) {
        UpdateView();
    }
    return m_view;
}

glm::mat4 Camera::GetProjection() const { return m_projection; }

glm::mat4 Camera::GetViewPorjection() { return GetProjection() * GetView(); }

glm::mat4 Camera::GetViewPorjection() const {
    return GetProjection() * GetView();
}

bool Camera::Outdated() const { return m_outdated; }

void Camera::UpdateView() {
    m_position = GetWorldPosition();
    m_rotation = GetWorldRotation();

    m_view =
        glm::lookAt(m_position, m_position - GetWorldFront(), GetWorldUp());

    m_outdated = false;
}

glm::vec3 Camera::MapClipToWorld(const glm::vec2 &pos) const {
    glm::mat4 projectionView = GetViewPorjection();
    glm::vec4 worldPos =
        glm::inverse(projectionView) * glm::vec4(pos, 0.f, 1.0f);
    worldPos /= worldPos.w;
    return worldPos;
}

glm::vec3 Camera::MapWorldToClip(const glm::vec3 &pos) const {
    glm::mat4 projectionView = GetViewPorjection();
    glm::vec4 clipPos = projectionView * glm::vec4(pos, 1.0f);
    clipPos /= clipPos.w;
    return clipPos;
}

OrthographicCamera::OrthographicCamera(float width, float height, float zNear,
                                       float zFar)
    : Camera(zNear, zFar), m_width(width), m_height(height) {
    SetProjection(glm::ortho(-m_width / 2.f, m_width / 2.f, -m_height / 2.f,
                             m_height / 2.f, m_zNear, m_zFar));
}

OrthographicCamera::OrthographicCamera(Transform *transform, float width,
                                       float height, float zNear, float zFar)
    : Camera(transform, zNear, zFar), m_width(width), m_height(height) {
    SetProjection(glm::ortho(-m_width / 2.f, m_width / 2.f, -m_height / 2.f,
                             m_height / 2.f, m_zNear, m_zFar));
}

OrthographicCamera::OrthographicCamera(const glm::vec3 &position,
                                       const glm::quat &rotation, float width,
                                       float height, float zNear, float zFar)
    : Camera(position, rotation, zNear, zFar),
      m_width(width),
      m_height(height) {
    SetProjection(glm::ortho(-m_width / 2.f, m_width / 2.f, -m_height / 2.f,
                             m_height / 2.f, m_zNear, m_zFar));
}

void OrthographicCamera::Resize(float width, float height) {
    m_width = width;
    m_height = height;
    SetProjection(glm::ortho(-m_width / 2.f, m_width / 2.f, -m_height / 2.f,
                             m_height / 2.f, m_zNear, m_zFar));
}

PerspectiveCamera::PerspectiveCamera(float fov, float width, float height,
                                     float zNear, float zFar)
    : Camera(zNear, zFar), m_fov(fov), m_aspect(width / height) {
    SetProjection(glm::perspective(m_fov, m_aspect, m_zNear, m_zFar));
}

PerspectiveCamera::PerspectiveCamera(float fov, float width, float height,
                                     float zNear, float zFar,
                                     Transform *transform)
    : Camera(transform, zNear, zFar), m_fov(fov), m_aspect(width / height) {
    SetProjection(glm::perspective(m_fov, m_aspect, m_zNear, m_zFar));
}

PerspectiveCamera::PerspectiveCamera(float fov, float width, float height,
                                     float zNear, float zFar,
                                     const glm::vec3 &position,
                                     const glm::quat &rotation)
    : Camera(position, rotation, zNear, zFar),
      m_fov(fov),
      m_aspect(width / height) {
    SetProjection(glm::perspective(m_fov, m_aspect, m_zNear, m_zFar));
}

void PerspectiveCamera::Resize(float width, float height) {
    m_aspect = width / height;
    SetProjection(glm::perspective(m_fov, m_aspect, m_zNear, m_zFar));
}

}  // namespace SD
