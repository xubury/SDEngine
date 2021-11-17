#include "Utility/Transform.hpp"
#include "Utility/Log.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace SD {

bool Decompose(const glm::mat4 &transform, glm::vec3 &translation,
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

Transform *Transform::GetParent() { return m_parent; }

const Transform *Transform::GetParent() const { return m_parent; }

std::set<Transform *> &Transform::GetChildren() { return m_children; }

const std::set<Transform *> &Transform::GetChildren() const {
    return m_children;
}

void Transform::AddChild(Transform *child) {
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

void Transform::RemoveChild(Transform *child) {
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

void Transform::TranslateLocal(const glm::vec3 &t) {
    m_position += GetWorldRotation() * t;
    UpdateLocalPosition();
    for (Transform *child : m_children) {
        child->UpdateGlobalPosition();
    }
}

void Transform::TranslateWorld(const glm::vec3 &t) {
    m_position += t;
    UpdateLocalPosition();
    for (Transform *child : m_children) {
        child->UpdateGlobalPosition();
    }
}

void Transform::RotateLocal(const glm::quat &rotation) {
    m_rotation = GetWorldRotation() * rotation;
    UpdateLocalRotation();
    for (Transform *child : m_children) {
        child->UpdateGlobalRotation();
    }
}

void Transform::RotateWorld(const glm::quat &rotation) {
    m_rotation = rotation * GetWorldRotation();
    UpdateLocalRotation();
    for (Transform *child : m_children) {
        child->UpdateGlobalRotation();
    }
}

void Transform::SetLocalPosition(const glm::vec3 &position) {
    m_localPosition = position;
    UpdateGlobalPosition();
    for (Transform *child : m_children) {
        child->UpdateGlobalPosition();
    }
}

void Transform::SetLocalRotation(const glm::quat &rotation) {
    m_localRotation = rotation;
    UpdateGlobalRotation();
    for (Transform *child : m_children) {
        child->UpdateGlobalRotation();

        child->UpdateGlobalPosition();
    }
}

void Transform::SetLocalScale(const glm::vec3 &scale) {
    m_localScale = scale;
    UpdateGlobalScale();
    for (Transform *child : m_children) {
        child->UpdateGlobalScale();

        child->UpdateGlobalPosition();
    }
}

void Transform::SetLocalTransform(const glm::mat4 &transform) {
    Decompose(transform, m_localPosition, m_localRotation, m_localScale);
    UpdateGlobalScale();
    UpdateGlobalRotation();
    UpdateGlobalPosition();
    for (Transform *child : m_children) {
        child->UpdateGlobalScale();
        child->UpdateGlobalRotation();
        child->UpdateGlobalPosition();
    }
}

glm::vec3 Transform::GetLocalPosition() const { return m_localPosition; }

glm::quat Transform::GetLocalRotation() const { return m_localRotation; }

glm::vec3 Transform::GetLocalScale() const { return m_localScale; }

glm::mat4 Transform::GetLocalTransform() const {
    return glm::translate(glm::mat4(1.0f), m_localPosition) *
           glm::toMat4(m_localRotation) *
           glm::scale(glm::mat4(1.0f), m_localScale);
}

void Transform::SetWorldPosition(const glm::vec3 &position) {
    m_position = position;
    UpdateLocalPosition();
    for (Transform *child : m_children) {
        child->UpdateGlobalPosition();
    }
}

void Transform::SetWorldRotation(const glm::quat &rotation) {
    m_rotation = rotation;
    UpdateLocalRotation();
    for (Transform *child : m_children) {
        child->UpdateGlobalRotation();

        child->UpdateGlobalPosition();
    }
}

void Transform::SetWorldScale(const glm::vec3 &scale) {
    m_scale = scale;
    UpdateLocalScale();
    for (Transform *child : m_children) {
        child->UpdateGlobalScale();

        child->UpdateGlobalPosition();
    }
}

void Transform::SetWorldTransform(const glm::mat4 &transform) {
    Decompose(transform, m_position, m_rotation, m_scale);
    UpdateLocalPosition();
    UpdateLocalRotation();
    UpdateLocalScale();
    for (Transform *child : m_children) {
        child->UpdateGlobalPosition();
        child->UpdateGlobalRotation();
        child->UpdateGlobalScale();
    }
}

glm::vec3 Transform::GetWorldPosition() const { return m_position; }

glm::quat Transform::GetWorldRotation() const { return m_rotation; }

glm::vec3 Transform::GetWorldScale() const { return m_scale; }

glm::mat4 Transform::GetWorldTransform() const {
    return glm::translate(glm::mat4(1.0f), m_position) *
           glm::toMat4(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
}

glm::vec3 Transform::GetLocalRight() const {
    return m_localRotation * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::GetLocalUp() const {
    return m_localRotation * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::GetLocalFront() const {
    return m_localRotation * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 Transform::GetWorldRight() const {
    return m_rotation * glm::vec3(1.f, 0.f, 0.f);
}

glm::vec3 Transform::GetWorldUp() const {
    return m_rotation * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Transform::GetWorldFront() const {
    return m_rotation * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 Transform::ToLocalSpace(const glm::vec3 &world) const {
    glm::mat3 rotation = glm::toMat3(GetWorldRotation());
    return glm::transpose(rotation) * (world - GetLocalPosition());
}

glm::vec3 Transform::ToWorldSpace(const glm::vec3 &local) const {
    return GetWorldRotation() * local + GetWorldPosition();
}

glm::vec3 Transform::ToLocalVector(const glm::vec3 &worldVec) const {
    return glm::transpose(glm::toMat3(GetWorldRotation())) * worldVec;
}

glm::vec3 Transform::ToWorldVector(const glm::vec3 &localVec) const {
    return GetWorldRotation() * localVec;
}

void Transform::UpdateGlobalPosition() {
    if (m_parent == nullptr)
        m_position = m_localPosition;
    else {
        glm::mat4 global = m_parent->GetWorldTransform() * GetLocalTransform();
        m_position = global[3];
    }
    for (Transform *child : m_children) {
        child->UpdateGlobalPosition();
    }
}

void Transform::UpdateGlobalRotation() {
    if (m_parent == nullptr)
        m_rotation = m_localRotation;
    else {
        glm::mat4 global =
            glm::mat4(m_parent->m_rotation) * glm::toMat4(GetLocalRotation());
        m_rotation = glm::quat(global);
    }
    for (Transform *child : m_children) {
        child->UpdateGlobalRotation();
    }
}

void Transform::UpdateGlobalScale() {
    if (m_parent == nullptr)
        m_scale = m_localScale;
    else {
        glm::mat4 global = glm::scale(glm::mat4(1.0f), m_parent->m_scale) *
                           glm::scale(glm::mat4(1.0f), m_localScale);
        m_scale = glm::vec3(global[0][0], global[1][1], global[2][2]);
    }
    for (Transform *child : m_children) {
        child->UpdateGlobalScale();
    }
}

void Transform::UpdateLocalPosition() {
    if (m_parent == nullptr)
        m_localPosition = m_position;
    else {
        glm::mat4 global =
            glm::inverse(m_parent->GetWorldTransform()) * GetWorldTransform();
        m_localPosition = global[3];
    }
}

void Transform::UpdateLocalRotation() {
    if (m_parent == nullptr)
        m_localRotation = m_rotation;
    else {
        glm::mat4 global = glm::inverse(glm::toMat4(m_parent->m_rotation)) *
                           glm::toMat4(GetWorldRotation());
        m_localRotation = glm::quat(global);
    }
}

void Transform::UpdateLocalScale() {
    if (m_parent == nullptr)
        m_localScale = m_scale;
    else {
        glm::mat4 global =
            glm::inverse(glm::scale(glm::mat4(1.0f), m_parent->m_scale)) *
            glm::scale(glm::mat4(1.0f), m_scale);
        m_localScale = glm::vec3(global[0][0], global[1][1], global[2][2]);
    }
}

}  // namespace SD
