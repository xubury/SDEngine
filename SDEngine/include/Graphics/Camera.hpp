#ifndef SD_CAMERA_HPP
#define SD_CAMERA_HPP

#include "Graphics/Export.hpp"
#include "Utility/Base.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"

#include "Graphics/Graphics.hpp"

namespace SD {

enum class CameraType { Orthographic, Perspective };

class SD_GRAPHICS_API Camera {
   public:
    Camera();
    Camera(CameraType type, float fov, float width, float height, float near_z,
           float far_z);

    void Resize(float width, float height);

    void TranslateLocal(const glm::vec3 &t);
    void TranslateWorld(const glm::vec3 &t);
    void RotateLocal(const glm::quat &r);
    void RotateWorld(const glm::quat &r);

    glm::vec3 GetWorldRight() const;
    glm::vec3 GetWorldUp() const;
    glm::vec3 GetWorldFront() const;

    void SetWorldPosition(const glm::vec3 &position);
    void SetWorldRotation(const glm::quat &rotation);

    glm::vec3 GetWorldPosition() const;

    glm::quat GetWorldRotation() const;

    glm::mat4 GetWorldTransform() const;

    void SetWorldTransform(const glm::mat4 &transform);

    void UpdateProjection();

    glm::mat4 GetView();
    glm::mat4 GetView() const;

    glm::mat4 GetProjection();
    glm::mat4 GetProjection() const;

    glm::mat4 GetViewPorjection();
    glm::mat4 GetViewPorjection() const;

    bool ViewOutdated() const;
    bool ProjectionOutdated() const;

    void UpdateView();

    glm::vec3 MapClipToWorld(const glm::vec2 &pos) const;
    glm::vec3 MapWorldToClip(const glm::vec3 &pos) const;
    Math::Ray ComputeCameraRay(const glm::vec2 &clip) const;

    void SetFOV(float fov);
    float GetFOV() const;

    void SetNearWidth(float width);
    void SetNearHeight(float height);

    float GetNearWidth() const;
    float GetNearHeight() const;

    float GetFarWidth() const;
    float GetFarHeight() const;

    void SetNearZ(float near_z);
    float GetNearZ() const;

    void SetFarZ(float far_z);
    float GetFarZ() const;

    void SetCameraType(CameraType type);
    CameraType GetCameraType() const;

    SERIALIZE(m_type, m_position, m_rotation, m_fov, m_near_width,
              m_near_height, m_near_z, m_far_z)
   private:
    glm::mat4 m_view;
    glm::mat4 m_projection;

    CameraType m_type;
    glm::vec3 m_position;
    glm::quat m_rotation;

    float m_fov;
    float m_near_width;
    float m_near_height;
    float m_near_z;
    float m_far_z;

    bool m_view_outdated;
    bool m_projection_outdated;
};

}  // namespace SD

#endif /* SD_CAMERA_HPP */
