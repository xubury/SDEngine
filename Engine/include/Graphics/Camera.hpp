#ifndef SD_CAMERA_HPP
#define SD_CAMERA_HPP

#include "Utility/Export.hpp"
#include "Utility/Transform.hpp"

namespace sd {

class SD_API Camera {
   public:
    Camera();
    Camera(Transform *transform);
    Camera(const glm::vec3 &position, const glm::quat &rotation);

    virtual void resize(float width, float height) = 0;

    void translateLocal(const glm::vec3 &t);
    void translateWorld(const glm::vec3 &t);
    void rotateLocal(const glm::quat &r);
    void rotateWorld(const glm::quat &r);

    glm::vec3 getLocalRight() const;
    glm::vec3 getLocalUp() const;
    glm::vec3 getLocalFront() const;

    glm::vec3 getWorldRight() const;
    glm::vec3 getWorldUp() const;
    glm::vec3 getWorldFront() const;

    void setTransform(Transform *transform);
    void setLocalPosition(const glm::vec3 &position);
    void setLocalRotation(const glm::quat &rotation);

    void setWorldPosition(const glm::vec3 &position);
    void setWorldRotation(const glm::quat &rotation);

    glm::vec3 getLocalPosition() const;
    glm::vec3 getWorldPosition() const;

    glm::quat getLocalRotation() const;
    glm::quat getWorldRotation() const;

    glm::mat4 getLocalTransform() const;
    glm::mat4 getWorldTransform() const;

    void setWorldTransform(const glm::mat4 &transform);
    void setLocalTransform(const glm::mat4 &transform);

    void setProjection(const glm::mat4 &projection);

    glm::mat4 getView();
    glm::mat4 getView() const;

    glm::mat4 getProjection() const;

    glm::mat4 getViewPorjection();
    glm::mat4 getViewPorjection() const;

    bool outdated() const;

    glm::vec3 mapClipToWorld(const glm::vec2 &pos) const;
    glm::vec3 mapWorldToClip(const glm::vec3 &pos) const;

    void updateView();

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

    void setProjection(float width, float height, float zNear, float zFar);

    void resize(float width, float height) override;

   private:
    float m_width;
    float m_height;
    float m_zNear;
    float m_zFar;
};

class SD_API PerspectiveCamera : public Camera {
   public:
    PerspectiveCamera(float fov, float aspect, float zNear, float zFar);
    PerspectiveCamera(Transform *m_transform, float fov, float aspect,
                      float zNear, float zFar);
    PerspectiveCamera(const glm::vec3 &position, const glm::quat &rotation,
                      float fov, float aspect, float zNear, float zFar);
    ~PerspectiveCamera() = default;

    void setProjection(float fov, float aspect, float zNear, float zFar);

    void resize(float width, float height) override;

    float getFOV() const { return m_fov; }

    float getAspect() const { return m_aspect; }

    float getNearZ() const { return m_zNear; }
    float getFarZ() const { return m_zFar; }
   private:
    float m_fov;
    float m_aspect;
    float m_zNear;
    float m_zFar;
};

}  // namespace sd

#endif /* SD_CAMERA_HPP */
