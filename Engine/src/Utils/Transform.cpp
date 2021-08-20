#include "Utils/Transform.hpp"
#include "Common/Log.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace sd {

Transform::Transform()
    : m_position(0.f),
      m_rotation(1.f, 0.f, 0.f, 0.f),
      m_scale(1.0f),
      m_localPosition(0.f),
      m_localRotation(1.f, 0.f, 0.f, 0.f),
      m_localScale(1.0f),
      m_parent(nullptr) {}

Transform *Transform::getParent() { return m_parent; }

const Transform *Transform::getParent() const { return m_parent; }

std::set<Transform *> &Transform::getChildren() { return m_children; }

const std::set<Transform *> &Transform::getChildren() const {
    return m_children;
}

void Transform::addChild(Transform *child) {
    if (child == nullptr) {
        SD_CORE_ERROR("[Transform::addChild] Tried to add a null child");
        return;
    }
    if (child == this) {
        SD_CORE_ERROR("[Transform::addChild] Tried to add its own as child");
        return;
    }
    child->m_parent = this;
    m_children.emplace(child);
}

void Transform::removeChild(Transform *child) {
    if (m_children.find(child) == m_children.end()) {
        SD_CORE_ERROR(
            "[Transform::removeChild] Tried to remove a non-existent child");
        return;
    }
    if (child == nullptr) {
        SD_CORE_ERROR("[Transform::removeChild] Tried to remove a null child");
        return;
    }

    child->m_parent = nullptr;
    m_children.erase(child);
}

void Transform::apply(const Transform &transform) {
    m_position += transform.m_position;
    m_rotation *= transform.m_rotation;
    m_scale *= transform.m_scale;
    updateLocalScale();
    updateLocalRotation();
    updateLocalPosition();
}

void Transform::translateLocal(const glm::vec3 &t) {
    m_localPosition += t;
    updateGlobalPosition();
}

void Transform::translateWorld(const glm::vec3 &t) {
    m_position += t;
    updateLocalPosition();
}

void Transform::rotateLocal(const glm::quat &rotation) {
    m_localRotation = rotation * m_localRotation;
    updateGlobalPosition();
}

void Transform::rotateWorld(const glm::quat &rotation) {
    m_rotation = rotation * m_rotation;
    updateLocalRotation();
}

void Transform::setLocalPosition(const glm::vec3 &position) {
    m_localPosition = position;
    updateGlobalPosition();
    for (Transform *child : m_children) {
        child->updateGlobalPosition();
    }
}

void Transform::setLocalRotation(const glm::quat &rotation) {
    m_localRotation = rotation;
    updateGlobalRotation();
    for (Transform *child : m_children) {
        child->updateGlobalRotation();

        child->updateGlobalPosition();
    }
}

void Transform::setLocalScale(const glm::vec3 &scale) {
    m_localScale = scale;
    updateGlobalScale();
    for (Transform *child : m_children) {
        child->updateGlobalScale();

        child->updateGlobalPosition();
    }
}

void Transform::setLocalTransform(const glm::mat4 &transform) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, m_localScale, m_localRotation, m_localPosition,
                   skew, perspective);
    updateGlobalScale();
    updateGlobalRotation();
    updateGlobalPosition();
    for (Transform *child : m_children) {
        child->updateGlobalScale();
        child->updateGlobalRotation();
        child->updateGlobalPosition();
    }
}

glm::vec3 Transform::getLocalPosition() const { return m_localPosition; }

glm::quat Transform::getLocalRotation() const { return m_localRotation; }

glm::vec3 Transform::getLocalScale() const { return m_localScale; }

glm::vec3 Transform::getLocalEulerAngle() const {
    return glm::eulerAngles(getLocalRotation());
}

glm::mat4 Transform::getLocalTransform() const {
    return glm::translate(glm::mat4(1.0f), m_localPosition) *
           glm::toMat4(m_localRotation) *
           glm::scale(glm::mat4(1.0f), m_localScale);
}

void Transform::setWorldPosition(const glm::vec3 &position) {
    m_position = position;
    updateLocalPosition();
    for (Transform *child : m_children) {
        child->updateGlobalPosition();
    }
}

void Transform::setWorldRotation(const glm::quat &rotation) {
    m_rotation = rotation;
    updateLocalRotation();
    for (Transform *child : m_children) {
        child->updateGlobalRotation();

        child->updateGlobalPosition();
    }
}

void Transform::setWorldScale(const glm::vec3 &scale) {
    m_scale = scale;
    updateLocalScale();
    for (Transform *child : m_children) {
        child->updateGlobalScale();

        child->updateGlobalPosition();
    }
}

void Transform::setWorldTransform(const glm::mat4 &transform) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, m_scale, m_rotation, m_position, skew,
                   perspective);
    updateLocalScale();
    updateLocalRotation();
    updateLocalPosition();
}

glm::vec3 Transform::getWorldPosition() const { return m_position; }

glm::quat Transform::getWorldRotation() const { return m_rotation; }

glm::vec3 Transform::getWorldScale() const { return m_scale; }

glm::mat4 Transform::getWorldTransform() const {
    return glm::translate(glm::mat4(1.0f), m_position) *
           glm::toMat4(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
}

glm::vec3 Transform::getRight() const {
    return m_localRotation * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::getUp() const {
    return m_localRotation * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::getFront() const {
    return m_localRotation * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 Transform::getWorldRight() const {
    return m_rotation * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::getWorldUp() const {
    return m_rotation * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::getWorldFront() const {
    return m_rotation * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 Transform::toLocalSpace(const glm::vec3 &world) const {
    glm::mat3 rotation = glm::toMat3(getWorldRotation());
    return glm::transpose(rotation) * (world - getLocalPosition());
}

glm::vec3 Transform::toWorldSpace(const glm::vec3 &local) const {
    return getWorldRotation() * local + getWorldPosition();
}

glm::vec3 Transform::toLocalVector(const glm::vec3 &worldVec) const {
    return glm::transpose(glm::toMat3(getWorldRotation())) * worldVec;
}

glm::vec3 Transform::toWorldVector(const glm::vec3 &localVec) const {
    return getWorldRotation() * localVec;
}

void Transform::updateGlobalPosition() {
    if (m_parent == nullptr)
        m_position = m_localPosition;
    else {
        glm::mat4 global = m_parent->getWorldTransform() * getLocalTransform();
        m_position = global[3];
    }
}

void Transform::updateGlobalRotation() {
    if (m_parent == nullptr)
        m_rotation = m_localRotation;
    else {
        glm::mat4 global =
            glm::mat4(m_parent->getWorldRotation()) * getLocalTransform();
        m_rotation = glm::quat(global);
    }
}

void Transform::updateGlobalScale() {
    if (m_parent == nullptr)
        m_scale = m_localScale;
    else {
        glm::mat4 global = glm::scale(glm::mat4(1.0f), m_parent->m_scale) *
                           glm::scale(glm::mat4(1.0f), m_localScale);
        m_scale = glm::vec3(global[0][0], global[1][1], global[2][2]);
    }
}

void Transform::updateLocalPosition() {
    if (m_parent == nullptr)
        m_localPosition = m_position;
    else {
        glm::mat4 global =
            glm::inverse(m_parent->getWorldTransform()) * getWorldTransform();
        m_localPosition = global[3];
    }
}

void Transform::updateLocalRotation() {
    if (m_parent == nullptr)
        m_localRotation = m_rotation;
    else {
        glm::mat4 global =
            glm::inverse(glm::mat4(m_parent->m_rotation)) * getWorldTransform();
        m_localRotation = glm::quat(global);
    }
}

void Transform::updateLocalScale() {
    if (m_parent == nullptr)
        m_localScale = m_scale;
    else {
        glm::mat4 global =
            glm::inverse(glm::scale(glm::mat4(1.0f), m_parent->m_scale)) *
            glm::scale(glm::mat4(1.0f), m_scale);
        m_localScale = glm::vec3(global[0][0], global[1][1], global[2][2]);
    }
}

}  // namespace sd
