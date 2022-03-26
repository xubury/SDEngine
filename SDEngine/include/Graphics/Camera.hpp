#ifndef SD_CAMERA_HPP
#define SD_CAMERA_HPP

#include "Graphics/Export.hpp"
#include "Utility/Base.hpp"
#include "Utility/Transform.hpp"
#include "Utility/Math.hpp"

#include "Graphics/Graphics.hpp"

namespace SD {

enum class CameraType : int32_t { Orthographic, Perspective };

class SD_GRAPHICS_API Camera {
   public:
    Camera();
    Camera(CameraType type, float fov, float width, float height, float near_z,
           float far_z);

    void Resize(float width, float height);

    void TranslateLocal(const Vector3f &t);
    void TranslateWorld(const Vector3f &t);
    void RotateLocal(const Quaternion &r);
    void RotateWorld(const Quaternion &r);

    Vector3f GetWorldRight() const;
    Vector3f GetWorldUp() const;
    Vector3f GetWorldFront() const;

    void SetWorldPosition(const Vector3f &position);
    void SetWorldRotation(const Quaternion &rotation);

    Vector3f GetWorldPosition() const;

    Quaternion GetWorldRotation() const;

    Matrix4f GetWorldTransform() const;

    void SetWorldTransform(const Matrix4f &transform);

    void UpdateProjection();

    Matrix4f GetView();
    Matrix4f GetView() const;

    Matrix4f GetProjection();
    Matrix4f GetProjection() const;

    Matrix4f GetViewPorjection();
    Matrix4f GetViewPorjection() const;

    bool ViewOutdated() const;
    bool ProjectionOutdated() const;

    void UpdateView();

    Vector3f MapClipToWorld(const Vector2f &pos) const;
    Vector3f MapWorldToClip(const Vector3f &pos) const;
    Math::Ray ComputeCameraRay(const Vector2f &clip) const;

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
    Matrix4f m_view;
    Matrix4f m_projection;

    CameraType m_type;
    Vector3f m_position;
    Quaternion m_rotation;

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
