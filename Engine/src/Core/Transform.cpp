#include "Core/Transform.hpp"
#include "Utils/Log.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace sd {

Transform::Transform()
    : m_position(0.f), m_rotation(1.f, 0.f, 0.f, 0.f), m_scale(1.0f) {}

Transform *Transform::parent() { return m_parent; }

const Transform *Transform::parent() const { return m_parent; }

std::vector<Transform *> &Transform::children() { return m_children; }

const std::vector<Transform *> &Transform::children() const {
    return m_children;
}

void Transform::setParent(Transform *transform, bool cleanTree) {
    if (m_parent == transform) {
        return;
    }

    if (cleanTree && m_parent) {
        m_parent->removeChild(this);
    }

    m_parent = transform;

    if (cleanTree && m_parent) {
        m_parent->addChild(this);
    }
}

void Transform::addChild(Transform *transform) {
    if (std::find(m_children.begin(), m_children.end(), transform) !=
        m_children.end()) {
        SD_CORE_ERROR("[Transform::addChild] Tried to add duplicate child");
        return;
    }
    if (transform == nullptr) {
        SD_CORE_ERROR("[Transform::addChild] Tried to add a null child");
        return;
    }
    transform->setParent(this);
    m_children.push_back(transform);
}

void Transform::removeChild(Transform *transform) {
    auto iter = std::find(m_children.begin(), m_children.end(), transform);
    if (iter == m_children.end()) {
        SD_CORE_ERROR(
            "[Transform::removeChild] Tried to remove a non-existent child");
        return;
    }
    if (transform == nullptr) {
        SD_CORE_ERROR("[Transform::removeChild] Tried to remove a null child");
        return;
    }

    transform->setParent(nullptr);
    m_children.erase(iter);
}

void Transform::apply(const Transform &transform) {
    m_position += transform.m_position;
    m_rotation *= transform.m_rotation;
    m_scale *= transform.m_scale;
}

void Transform::translateLocal(const glm::vec3 &t) {
    m_position += getWorldRotation() * t;
}

void Transform::translateWorld(const glm::vec3 &t) { m_position += t; }

void Transform::rotateLocal(float radians, const glm::vec3 &axis) {
    m_rotation = getWorldRotation() * glm::angleAxis(radians, axis);
}

void Transform::rotateWorld(float radians, const glm::vec3 &axis) {
    m_rotation = glm::angleAxis(radians, axis) * getWorldRotation();
}

void Transform::rotateLocal(const glm::quat &rotation) {
    m_rotation = getWorldRotation() * rotation;
}

void Transform::rotateWorld(const glm::quat &rotation) {
    m_rotation = rotation * getWorldRotation();
}

void Transform::setPosition(const glm::vec3 &position) {
    m_position = position;
}

void Transform::setRotation(const glm::quat &rotation) {
    m_rotation = rotation;
}

void Transform::setEulerAngle(glm::vec3 eulerAngle) {
    m_rotation = glm::quat(eulerAngle);
}

void Transform::setTransform(const glm::mat4 &transform) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, m_scale, m_rotation, m_position, skew,
                   perspective);
}

glm::vec3 Transform::getPosition() const { return m_position; }

glm::quat Transform::getRotation() const { return m_rotation; }

glm::vec3 Transform::getScale() const { return m_scale; }

glm::vec3 Transform::getEulerAngle() const {
    return glm::eulerAngles(m_rotation);
}

glm::mat4 Transform::getTransform() const {
    glm::mat4 rotation = glm::toMat4(m_rotation);

    return glm::translate(glm::mat4(1.0f), m_position) * rotation *
           glm::scale(glm::mat4(1.0f), m_scale);
}

glm::vec3 Transform::getWorldPosition() const {
    return getPosition() + getParentPosition();
}

glm::quat Transform::getWorldRotation() const {
    return getRotation() * getParentRotation();
}

glm::vec3 Transform::getWorldScale() const {
    return getScale() * getParentScale();
}

glm::vec3 Transform::getWorldEulerAngle() const {
    return glm::eulerAngles(getWorldRotation());
}

glm::mat4 Transform::getWorldTransform() const {
    glm::mat4 parent(1.0f);
    if (m_parent) {
        parent = m_parent->getWorldTransform();
    }
    return parent * getTransform();
}

glm::vec3 Transform::getRight() const {
    return m_rotation * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::getUp() const {
    return m_rotation * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::getFront() const {
    return m_rotation * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 Transform::getWorldRight() const {
    glm::vec3 parent(1.0f);
    if (m_parent) {
        parent = m_parent->getWorldRight();
    }
    return getRight() * parent;
}

glm::vec3 Transform::getWorldUp() const {
    glm::vec3 parent(1.0f);
    if (m_parent) {
        parent = m_parent->getWorldUp();
    }
    return getUp() * parent;
}

glm::vec3 Transform::getWorldFront() const {
    glm::vec3 parent(1.0f);
    if (m_parent) {
        parent = m_parent->getWorldFront();
    }
    return getFront() * parent;
}

glm::vec3 Transform::toLocalSpace(const glm::vec3 &world) const {
    glm::mat3 rotation = glm::toMat3(getWorldRotation());
    return glm::transpose(rotation) * (world - getPosition());
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

glm::vec3 Transform::getParentPosition() const {
    glm::vec3 parent(0);
    if (m_parent) {
        parent = m_parent->getWorldPosition();
    }
    return parent;
}

glm::quat Transform::getParentRotation() const {
    glm::quat parent(1, 0, 0, 0);
    if (m_parent) {
        parent = getWorldRotation();
    }
    return parent;
}

glm::vec3 Transform::getParentScale() const {
    glm::vec3 parent(1.0f);
    if (m_parent) {
        parent = getWorldScale();
    }
    return parent;
}

}  // namespace sd
