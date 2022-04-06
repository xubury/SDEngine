#ifndef SD_GRAPHICS_LAYER_HPP
#define SD_GRAPHICS_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/DeferredRenderer.hpp"
#include "Renderer/SkyboxRenderer.hpp"
#include "Renderer/PostProcessRenderer.hpp"
#include "Renderer/SpriteRenderer.hpp"
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

    void OutputEntityBuffer(Framebuffer* framebuffer, int attachment);
    void OutputColorBuffer(Framebuffer* framebuffer, int attachment);

   private:
    void InitBuffers();

    Device* m_device;
    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;
    bool m_debug;

    Ref<Framebuffer> m_main_target;

    Ref<Texture> m_color_buffer;
    Framebuffer* m_color_output;
    int32_t m_color_output_attachment;

    Ref<Texture> m_entity_buffer;
    Framebuffer* m_entity_ouptut;
    int32_t m_entity_output_attachment;

    Ref<Renderbuffer> m_depth_buffer;

    Ref<Texture> m_light_icon;

    Camera* m_camera;
    Scene* m_scene;
    DeferredRenderer m_deferred_renderer;
    SkyboxRenderer m_skybox_renderer;
    SpriteRenderer m_sprite_renderer;
    PostProcessRenderer m_post_renderer;

    FPSCounter m_fps;
    std::string m_renderer2d_debug_str;
    std::string m_renderer3d_debug_str;
};

}  // namespace SD

#endif
