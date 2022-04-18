#ifndef SD_GRAPHICS_LAYER_HPP
#define SD_GRAPHICS_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/DeferredRenderer.hpp"
#include "Renderer/SkyboxRenderer.hpp"
#include "Renderer/PostProcessRenderer.hpp"
#include "Renderer/SpriteRenderer.hpp"
#include "Resource/Resource.hpp"
#include "Utility/Timing.hpp"

namespace SD {

class SD_CORE_API GraphicsLayer : public Layer {
   public:
    GraphicsLayer(Device* device, int32_t width, int32_t height,
                  MultiSampleLevel msaa);
    void OnInit() override;
    void OnDestroy() override;

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

    TextureHandle m_light_icon;

    Camera* m_camera;
    Scene* m_scene;

    FPSCounter m_fps;
    float m_deferred_time;
    float m_post_rendering_time;
};

}  // namespace SD

#endif
