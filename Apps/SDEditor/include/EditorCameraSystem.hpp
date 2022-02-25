#ifndef SD_EDITOR_CAMERA_SYSTEM_HPP
#define SD_EDITOR_CAMERA_SYSTEM_HPP

#include "Core/System.hpp"
#include "ECS/Entity.hpp"

namespace SD {

class EditorCameraSystem : public System {
   public:
    EditorCameraSystem(uint32_t width, uint32_t height);

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnTick(float dt) override;

    void OnSizeEvent(const ViewportEvent &event);
    void OnMouseMotion(const MouseMotionEvent &event);

    void Rotate(float yaw, float pitch);

    void AllowRotate(bool enable) { m_allow_rotate = enable; }

    Camera *GetCamera() { return &m_camera; }

   private:
    HandlerRegistration m_key_handler;
    HandlerRegistration m_size_handler;

    uint32_t m_width;
    uint32_t m_height;

    float m_pitch;

    glm::vec2 m_mouse_smooth_movement;
    glm::vec2 m_mouse_movement;

    Camera m_camera;

    bool m_allow_rotate{false};
};

}  // namespace SD
#endif
