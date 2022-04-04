#ifndef SD_EDITOR_CAMERA_SYSTEM_HPP
#define SD_EDITOR_CAMERA_SYSTEM_HPP

#include "Core/ECSSystem.hpp"
#include "Graphics/Camera.hpp"

namespace SD {

class EditorCameraSystem : public ECSSystem {
   public:
    EditorCameraSystem(int32_t width, int32_t height);

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnImGui() override;

    void OnTick(float dt) override;

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

    Vector2f m_mouse_smooth_movement;
    Vector2f m_mouse_movement;

    Camera m_camera;

    bool m_allow_rotate{true};
};

}  // namespace SD
#endif
