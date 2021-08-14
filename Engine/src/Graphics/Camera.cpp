#include "Graphics/Camera.hpp"

namespace sd {

Camera::Camera()
    : m_transform(nullptr),
      m_position(0.f),
      m_rotation(1.f, 0.f, 0.f, 0.f),
      m_outdated(true) {
    updateView();
}

Camera::Camera(Transform *transform)
    : m_transform(transform), m_position(0.f), m_rotation(1.f, 0.f, 0.f, 0.f) {
    updateView();
}

Camera::Camera(const glm::vec3 &position, const glm::quat &rotation)
    : m_transform(nullptr), m_position(position), m_rotation(rotation) {
    updateView();
}

Camera::~Camera() {}

void Camera::setTransform(Transform *transform) { m_transform = transform; }

void Camera::setPosition(const glm::vec3 &position) {
    if (getPosition() != position) {
        if (m_transform) {
            m_transform->setLocalPosition(position);
        } else {
            m_position = position;
        }
        m_outdated = true;
    }
}

void Camera::setRotation(const glm::quat &rotation) {
    if (getRotation() != rotation) {
        if (m_transform) {
            m_transform->setLocalRotation(rotation);
        } else {
            m_rotation = rotation;
        }
        m_outdated = true;
    }
}

glm::vec3 Camera::getPosition() const {
    return m_transform ? m_transform->getLocalPosition() : m_position;
}

glm::vec3 Camera::getWorldPosition() const {
    return m_transform ? m_transform->getWorldPosition() : m_position;
}

glm::quat Camera::getRotation() const {
    return m_transform ? m_transform->getLocalRotation() : m_rotation;
}

glm::quat Camera::getWorldRotation() const {
    return m_transform ? m_transform->getWorldRotation() : m_rotation;
}

void Camera::setProjection(const glm::mat4 &projection) {
    m_projection = projection;
}

glm::mat4 Camera::getView() const { return m_view; }

glm::mat4 Camera::getView() {
    if (outdated()) {
        updateView();
    }
    return m_view;
}

glm::mat4 Camera::getProjection() const { return m_projection; }

glm::mat4 Camera::getViewPorjection() { return getProjection() * getView(); }

glm::mat4 Camera::getViewPorjection() const {
    return getProjection() * getView();
}

bool Camera::outdated() const { return m_outdated; }

void Camera::updateView() {
    glm::vec3 pos = getWorldPosition();
    glm::vec3 front = getWorldRotation() * glm::vec3(0, 0, 1);
    glm::vec3 up = getWorldRotation() * glm::vec3(0, 1, 0);

    m_view = glm::lookAt(pos, pos - front, up);
    m_position = getWorldPosition();
    m_rotation = getWorldRotation();

    m_outdated = false;
}

glm::vec3 Camera::mapClipToWorld(const glm::vec2 &pos) const {
    glm::mat4 projectionView = getViewPorjection();
    glm::vec4 worldPos =
        glm::inverse(projectionView) * glm::vec4(pos, 0.f, 1.0f);
    worldPos /= worldPos.w;
    return worldPos;
}

glm::vec3 Camera::mapWorldToClip(const glm::vec3 &pos) const {
    glm::mat4 projectionView = getViewPorjection();
    glm::vec4 clipPos = projectionView * glm::vec4(pos, 1.0f);
    clipPos /= clipPos.w;
    return clipPos;
}

OrthographicCamera::OrthographicCamera(float width, float height, float zNear,
                                       float zFar) {
    setProjection(width, height, zNear, zFar);
}

OrthographicCamera::OrthographicCamera(Transform *transform, float width,
                                       float height, float zNear, float zFar)
    : Camera(transform) {
    setProjection(width, height, zNear, zFar);
}

OrthographicCamera::OrthographicCamera(const glm::vec3 &position,
                                       const glm::quat &rotation, float width,
                                       float height, float zNear, float zFar)
    : Camera(position, rotation) {
    setProjection(width, height, zNear, zFar);
}

void OrthographicCamera::setProjection(float width, float height, float zNear,
                                       float zFar) {
    Camera::setProjection(glm::ortho(-width / 2, width / 2, -height / 2,
                                     height / 2, zNear, zFar));
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float zNear,
                                     float zFar) {
    setProjection(fov, aspect, zNear, zFar);
}

PerspectiveCamera::PerspectiveCamera(Transform *transform, float fov,
                                     float aspect, float zNear, float zFar)
    : Camera(transform) {
    setProjection(fov, aspect, zNear, zFar);
}

PerspectiveCamera::PerspectiveCamera(const glm::vec3 &position,
                                     const glm::quat &rotation, float fov,
                                     float aspect, float zNear, float zFar)
    : Camera(position, rotation) {
    setProjection(fov, aspect, zNear, zFar);
}

void PerspectiveCamera::setProjection(float fov, float aspect, float zNear,
                                      float zFar) {
    Camera::setProjection(glm::perspective(fov, aspect, zNear, zFar));
}

}  // namespace sd
