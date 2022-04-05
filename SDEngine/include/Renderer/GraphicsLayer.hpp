#ifndef SD_GRAPHICS_LAYER_HPP
#define SD_GRAPHICS_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/CameraSystem.hpp"
#include "Renderer/LightingSystem.hpp"
#include "Renderer/SkyboxSystem.hpp"
#include "Renderer/PostProcessSystem.hpp"
#include "Renderer/SpriteRenderSystem.hpp"
#include "Utility/Timing.hpp"

namespace SD {

class SD_RENDERER_API GraphicsLayer : public Layer {
   public:
    GraphicsLayer(Device* device, int32_t width, int32_t height,
                  MultiSampleLevel msaa);
    void OnInit() override;

    void OnImGui() override;
    void OnRender() override;
    void OnTick(float dt) override;

    void SetRenderSize(int32_t width, int32_t height);
    void SetCamera(Camera* camera);
    void SetRenderScene(Scene* scene);
    void SetDebug(bool debug) { m_debug = debug; }

    Framebuffer* GetFramebuffer() { return m_main_target.get(); }

   private:
    void InitBuffers();

    Device* m_device;
    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;
    bool m_debug;
    Ref<Framebuffer> m_main_target;
    Ref<Texture> m_color_buffer;
    Ref<Texture> m_entity_buffer;
    Ref<Renderbuffer> m_depth_buffer;

    Ref<Texture> m_light_icon;

    Camera* m_camera;
    CameraSystem* m_camera_system;
    LightingSystem* m_lighting_system;
    SkyboxSystem* m_skybox_system;
    SpriteRenderSystem* m_sprite_system;
    PostProcessSystem* m_post_process_system;

    FPSCounter m_fps;
    std::string m_renderer2d_debug_str;
    std::string m_renderer3d_debug_str;
};

}  // namespace SD

#endif
