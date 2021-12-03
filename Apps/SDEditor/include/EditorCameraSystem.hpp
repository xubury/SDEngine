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

    void OnTick(float dt) override;

    void OnSizeEvent(const WindowSizeEvent &event);
    void OnMouseMotion(const MouseMotionEvent &event);

    void Rotate(float yaw, float pitch);

    Camera *GetCamera() const;

   private:
    HandlerRegistration m_key_handler;
    HandlerRegistration m_size_handler;

    uint32_t m_width;
    uint32_t m_height;

    float m_pitch;

    glm::vec2 m_mouse_smooth_movement;
    glm::vec2 m_mouse_movement;

    Scene m_editor_scene;
    Camera* m_camera;
};

}  // namespace SD
#endif
