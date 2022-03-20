#ifndef SD_GRAPHICS_LAYER_HPP
#define SD_GRAPHICS_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/CameraSystem.hpp"
#include "Renderer/LightingSystem.hpp"
#include "Renderer/SkyboxSystem.hpp"
#include "Renderer/PostProcessSystem.hpp"
#include "Renderer/SpriteRenderSystem.hpp"

namespace SD {

class SD_RENDERER_API GraphicsLayer : public Layer {
   public:
    GraphicsLayer(Device *device, int32_t width, int32_t height, MultiSampleLevel msaa);
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

    Device *m_device;
    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;
    bool m_debug;
    Ref<Framebuffer> m_main_framebuffer;

    Ref<Framebuffer> m_debug_gbuffer;
    Ref<Texture> m_light_icon;

    Camera* m_camera;
    CameraSystem* m_camera_system;
    LightingSystem* m_lighting_system;
    SkyboxSystem* m_skybox_system;
    SpriteRenderSystem* m_sprite_system;
    PostProcessSystem* m_post_process_system;

    HandlerRegistration m_size_handler;
    HandlerRegistration m_camera_handler;
};

}  // namespace SD

#endif
