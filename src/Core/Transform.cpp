#include "Core/Transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sd {

Transform::Transform()
    : m_position(0.f), m_rotation(1.f, 0.f, 0.f, 0.f), m_scale(1.0f) {}

void Transform::setTransform(const glm::mat4 &transform) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, m_scale, m_rotation, m_position, skew,
                   perspective);
}

void Transform::setRotation(const glm::quat &rotation) {
    m_rotation = rotation;
}

glm::quat Transform::getRotation() const { return m_rotation; }

void Transform::translateLocal(const glm::vec3 &t) {
    m_position += m_rotation * t;
}

void Transform::translateWorld(const glm::vec3 &t) { m_position += t; }

void Transform::rotateLocal(float radians, const glm::vec3 &axis) {
    m_rotation = m_rotation * glm::angleAxis(radians, axis);
}

void Transform::rotateWorld(float radians, const glm::vec3 &axis) {
    m_rotation = glm::angleAxis(radians, axis) * m_rotation;
}

void Transform::rotateLocal(const glm::quat &rotation) {
    m_rotation = m_rotation * rotation;
}

void Transform::rotateWorld(const glm::quat &rotation) {
    m_rotation = rotation * m_rotation;
}

glm::vec3 Transform::getEulerAngle() const {
    return glm::eulerAngles(m_rotation);
}

void Transform::setEulerAngle(glm::vec3 eulerAngle) {
    m_rotation = glm::quat(eulerAngle);
}

glm::mat4 Transform::getTransform() const {
    glm::mat4 rotation = glm::toMat4(m_rotation);

    return glm::translate(glm::mat4(1.0f), m_position) * rotation *
           glm::scale(glm::mat4(1.0f), m_scale);
}

void Transform::setPosition(const glm::vec3 &position) {
    m_position = position;
}

glm::vec3 Transform::getPosition() const { return m_position; }

glm::vec3 Transform::getLeft() const {
    return m_rotation * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::getUp() const {
    return m_rotation * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::getFront() const {
    return m_rotation * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 Transform::getScale() const { return m_scale; }

glm::vec3 Transform::toLocalSpace(const glm::vec3 &world) const {
    glm::mat3 rotation = glm::toMat3(m_rotation);
    return glm::transpose(rotation) * (world - getPosition());
}

glm::vec3 Transform::toWorldSpace(const glm::vec3 &local) const {
    return m_rotation * local + m_position;
}

glm::vec3 Transform::toLocalVector(const glm::vec3 &worldVec) const {
    glm::mat3 rotation = glm::toMat3(m_rotation);
    return glm::transpose(rotation) * worldVec;
}

glm::vec3 Transform::toWorldVector(const glm::vec3 &localVec) const {
    return m_rotation * localVec;
}

}  // namespace sd
