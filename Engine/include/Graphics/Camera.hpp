#ifndef SD_CAMERA_HPP
#define SD_CAMERA_HPP

#include "Common/Export.hpp"
#include "Core/Transform.hpp"

namespace sd {

class SD_API Camera {
   public:
    Camera();
    Camera(Transform *transform);
    Camera(const glm::vec3 &position, const glm::quat &rotation);

    virtual ~Camera() = 0;

    void setTransform(Transform *transform);
    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::quat &rotation);

    glm::vec3 getPosition() const;
    glm::vec3 getWorldPosition() const;

    glm::quat getRotation() const;
    glm::quat getWorldRotation() const;

    void setProjection(const glm::mat4 &projection);

    glm::mat4 getView();
    glm::mat4 getView() const;

    glm::mat4 getProjection() const;

    glm::mat4 getViewPorjection();
    glm::mat4 getViewPorjection() const;

    bool outdated() const;

    glm::vec3 mapClipToWorld(const glm::vec2 &pos) const;
    glm::vec3 mapWorldToClip(const glm::vec3 &pos) const;

   private:
    void updateView();

    glm::mat4 m_view;
    glm::mat4 m_projection;

    Transform *m_transform;

    glm::vec3 m_position;
    glm::quat m_rotation;

    bool m_outdated;
};

class SD_API OrthographicCamera : public Camera {
   public:
    OrthographicCamera() = default;
    OrthographicCamera(float width, float height, float zNear, float zFar);
    OrthographicCamera(Transform *m_transform, float width, float height,
                       float zNear, float zFar);
    OrthographicCamera(const glm::vec3 &position, const glm::quat &rotation,
                       float width, float height, float zNear, float zFar);
    ~OrthographicCamera() = default;

    void setProjection(float width, float height, float zNear, float zFar);
};

class SD_API PerspectiveCamera : public Camera {
   public:
    PerspectiveCamera() = default;
    PerspectiveCamera(float fov, float aspect, float zNear, float zFar);
    PerspectiveCamera(Transform *m_transform, float fov, float aspect,
                      float zNear, float zFar);
    PerspectiveCamera(const glm::vec3 &position, const glm::quat &rotation,
                      float fov, float aspect, float zNear, float zFar);
    ~PerspectiveCamera() = default;

    void setProjection(float fov, float aspect, float zNear, float zFar);
};

}  // namespace sd

#endif /* SD_CAMERA_HPP */
