#ifndef SD_EDITOR_CAMERA_HPP
#define SD_EDITOR_CAMERA_HPP

#include "Graphics/Camera.hpp"

namespace SD {

class EditorCamera : public Camera {
   public:
    EditorCamera(int32_t width, int32_t height);

    void ImGui();

    void Tick(float dt);

    void Move(int32_t x, int32_t y);

    void Rotate(float yaw, float pitch);

    void AllowRotate(bool enable) { m_allow_rotate = enable; }

   private:
    uint32_t m_width;
    uint32_t m_height;

    float m_pitch;

    Vector2f m_mouse_smooth_movement;
    Vector2f m_mouse_movement;

    bool m_allow_rotate{true};
};

}  // namespace SD
#endif
