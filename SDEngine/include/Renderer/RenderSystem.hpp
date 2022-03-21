#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "Core/ECSSystem.hpp"
#include "Renderer/Event.hpp"

namespace SD {

class RenderSystem : public ECSSystem {
   public:
    RenderSystem(const std::string &name) : ECSSystem(name), m_camera(nullptr) {}

    void OnInit() override
    {
        ECSSystem::OnInit();
        m_camera_handler = GetEventDispatcher().Register<CameraEvent>(
            [this](const CameraEvent &e) { m_camera = e.camera; });
    }

    void OnDestroy() override
    {
        GetEventDispatcher().RemoveHandler(m_camera_handler);
        ECSSystem::OnDestroy();
    }

    Camera &GetCamera() { return *m_camera; }

   private:
    HandlerRegistration m_camera_handler;
    Camera *m_camera;
};

}  // namespace SD
#endif
