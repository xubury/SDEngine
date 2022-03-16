#ifndef SD_GRAPHICS_LAYER_HPP
#define SD_GRAPHICS_LAYER_HPP

#include "Core/Layer.hpp"
#include "System/CameraSystem.hpp"
#include "System/LightingSystem.hpp"
#include "System/SkyboxSystem.hpp"
#include "System/ProfileSystem.hpp"
#include "System/PostProcessSystem.hpp"
#include "System/SpriteRenderSystem.hpp"

namespace SD {

class SD_SYSTEM_API GraphicsLayer : public Layer {
   public:
    GraphicsLayer(int32_t width, int32_t height, MultiSampleLevel msaa);
    void OnInit() override;

    void OnPush() override;
    void OnPop() override;

    void OnImGui() override;
    void OnRender() override;
    void OnTick(float dt) override;

    void Resize();

    void SetDebug(bool debug) { m_debug = debug; }

    Framebuffer* GetFramebuffer() { return m_main_framebuffer.get(); }

   private:
    void BlitGeometryBuffers();

    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;
    bool m_debug;
    Ref<Framebuffer> m_main_framebuffer;

    Ref<Framebuffer> m_debug_gbuffer;
    Ref<Texture> m_light_icon;

    CameraSystem* m_camera_system;
    LightingSystem* m_lighting_system;
    SkyboxSystem* m_skybox_system;
    SpriteRenderSystem* m_sprite_system;
    PostProcessSystem* m_post_process_system;

    HandlerRegistration m_viewport_handler;
};

}  // namespace SD

#endif