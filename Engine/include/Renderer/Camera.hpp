#ifndef SD_CAMERA_HPP
#define SD_CAMERA_HPP

#include "Utility/Base.hpp"
#include "Utility/Transform.hpp"

namespace SD {

class SD_API Camera {
   public:
    Camera(float zNear, float zFar);
    Camera(Transform *transform, float zNear, float zFar);
    Camera(const glm::vec3 &position, const glm::quat &rotation, float zNear,
           float zFar);

    virtual void Resize(float width, float height) = 0;

    void TranslateLocal(const glm::vec3 &t);
    void TranslateWorld(const glm::vec3 &t);
    void RotateLocal(const glm::quat &r);
    void RotateWorld(const glm::quat &r);

    glm::vec3 GetLocalRight() const;
    glm::vec3 GetLocalUp() const;
    glm::vec3 GetLocalFront() const;

    glm::vec3 GetWorldRight() const;
    glm::vec3 GetWorldUp() const;
    glm::vec3 GetWorldFront() const;

    void SetTransform(Transform *transform);
    void SetLocalPosition(const glm::vec3 &position);
    void SetLocalRotation(const glm::quat &rotation);

    void SetWorldPosition(const glm::vec3 &position);
    void SetWorldRotation(const glm::quat &rotation);

    glm::vec3 GetLocalPosition() const;
    glm::vec3 GetWorldPosition() const;

    glm::quat GetLocalRotation() const;
    glm::quat GetWorldRotation() const;

    glm::mat4 GetLocalTransform() const;
    glm::mat4 GetWorldTransform() const;

    void SetWorldTransform(const glm::mat4 &transform);
    void SetLocalTransform(const glm::mat4 &transform);

    void SetProjection(const glm::mat4 &projection);

    glm::mat4 GetView();
    glm::mat4 GetView() const;

    glm::mat4 GetProjection() const;

    glm::mat4 GetViewPorjection();
    glm::mat4 GetViewPorjection() const;

    bool Outdated() const;

    glm::vec3 MapClipToWorld(const glm::vec2 &pos) const;
    glm::vec3 MapWorldToClip(const glm::vec3 &pos) const;

    void UpdateView();

    virtual float GetNearWidth() const = 0;
    virtual float GetNearHeight() const = 0;

    virtual float GetFarWidth() const = 0;
    virtual float GetFarHeight() const = 0;

    float GetNearZ() const { return m_zNear; }
    float GetFarZ() const { return m_zFar; }

   protected:
    float m_zNear;
    float m_zFar;

   private:
    glm::mat4 m_view;
    glm::mat4 m_projection;

    Transform *m_transform;

    glm::vec3 m_position;
    glm::quat m_rotation;

    bool m_outdated;
};

class SD_API OrthographicCamera : public Camera {
   public:
    OrthographicCamera(float width, float height, float zNear, float zFar);
    OrthographicCamera(Transform *m_transform, float width, float height,
                       float zNear, float zFar);
    OrthographicCamera(const glm::vec3 &position, const glm::quat &rotation,
                       float width, float height, float zNear, float zFar);
    ~OrthographicCamera() = default;

    void Resize(float width, float height) override;

    float GetNearWidth() const override { return m_width; };
    float GetNearHeight() const override { return m_height; };

    float GetFarWidth() const override { return m_width; };
    float GetFarHeight() const override { return m_height; };

   private:
    float m_width;
    float m_height;
};

class SD_API PerspectiveCamera : public Camera {
   public:
    PerspectiveCamera(float fov, float width, float height, float zNear,
                      float zFar);
    PerspectiveCamera(float fov, float width, float height, float zNear,
                      float zFar, Transform *m_transform);
    PerspectiveCamera(float fov, float width, float height, float zNear,
                      float zFar, const glm::vec3 &position,
                      const glm::quat &rotation);
    ~PerspectiveCamera() = default;

    float GetFOV() const { return m_fov; }

    float GetAspect() const { return m_aspect; }

    void Resize(float width, float height) override;

    float GetNearWidth() const override { return GetNearHeight() * m_aspect; };
    float GetNearHeight() const override {
        return std::tan(m_fov / 2.f) * m_zNear * 2.f;
    };

    float GetFarWidth() const override { return GetFarHeight() * m_aspect; };
    float GetFarHeight() const override {
        return std::tan(m_fov / 2.f) * m_zFar * 2.f;
    };

   private:
    float m_fov;
    float m_aspect;
};

}  // namespace SD

#endif /* SD_CAMERA_HPP */
