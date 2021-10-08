#include "Utility/Transform.hpp"
#include "Utility/Log.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace sd {

bool decompose(const glm::mat4 &transform, glm::vec3 &translation,
               glm::quat &rotation, glm::vec3 &scale) {
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] =
            static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3];

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j) Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    int i, j, k = 0;
    T root, trace = Row[0].x + Row[1].y + Row[2].z;
    if (trace > static_cast<T>(0)) {
        root = sqrt(trace + static_cast<T>(1.0));
        rotation.w = static_cast<T>(0.5) * root;
        root = static_cast<T>(0.5) / root;
        rotation.x = root * (Row[1].z - Row[2].y);
        rotation.y = root * (Row[2].x - Row[0].z);
        rotation.z = root * (Row[0].y - Row[1].x);
    }  // End if > 0
    else {
        static int Next[3] = {1, 2, 0};
        i = 0;
        if (Row[1].y > Row[0].x) i = 1;
        if (Row[2].z > Row[i][i]) i = 2;
        j = Next[i];
        k = Next[j];

        root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));

        rotation[i] = static_cast<T>(0.5) * root;
        root = static_cast<T>(0.5) / root;
        rotation[j] = root * (Row[i][j] + Row[j][i]);
        rotation[k] = root * (Row[i][k] + Row[k][i]);
        rotation.w = root * (Row[j][k] - Row[k][j]);
    }  // End if <= 0

    return true;
}

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

void Transform::translateLocal(const glm::vec3 &t) {
    m_position += getWorldRotation() * t;
    updateLocalPosition();
    for (Transform *child : m_children) {
        child->updateGlobalPosition();
    }
}

void Transform::translateWorld(const glm::vec3 &t) {
    m_position += t;
    updateLocalPosition();
    for (Transform *child : m_children) {
        child->updateGlobalPosition();
    }
}

void Transform::rotateLocal(const glm::quat &rotation) {
    m_rotation = getWorldRotation() * rotation;
    updateLocalRotation();
    for (Transform *child : m_children) {
        child->updateGlobalRotation();
    }
}

void Transform::rotateWorld(const glm::quat &rotation) {
    m_rotation = rotation * getWorldRotation();
    updateLocalRotation();
    for (Transform *child : m_children) {
        child->updateGlobalRotation();
    }
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
    decompose(transform, m_localPosition, m_localRotation, m_localScale);
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
    decompose(transform, m_position, m_rotation, m_scale);
    updateLocalPosition();
    updateLocalRotation();
    updateLocalScale();
    for (Transform *child : m_children) {
        child->updateGlobalPosition();
        child->updateGlobalRotation();
        child->updateGlobalScale();
    }
}

glm::vec3 Transform::getWorldPosition() const { return m_position; }

glm::quat Transform::getWorldRotation() const { return m_rotation; }

glm::vec3 Transform::getWorldScale() const { return m_scale; }

glm::mat4 Transform::getWorldTransform() const {
    return glm::translate(glm::mat4(1.0f), m_position) *
           glm::toMat4(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
}

glm::vec3 Transform::getLocalRight() const {
    return m_localRotation * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::getLocalUp() const {
    return m_localRotation * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::getLocalFront() const {
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
    for (Transform *child : m_children) {
        child->updateGlobalPosition();
    }
}

void Transform::updateGlobalRotation() {
    if (m_parent == nullptr)
        m_rotation = m_localRotation;
    else {
        glm::mat4 global =
            glm::mat4(m_parent->m_rotation) * glm::toMat4(getLocalRotation());
        m_rotation = glm::quat(global);
    }
    for (Transform *child : m_children) {
        child->updateGlobalRotation();
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
    for (Transform *child : m_children) {
        child->updateGlobalScale();
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
        glm::mat4 global = glm::inverse(glm::toMat4(m_parent->m_rotation)) *
                           glm::toMat4(getWorldRotation());
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
