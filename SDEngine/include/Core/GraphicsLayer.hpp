#ifndef SD_GRAPHICS_LAYER_HPP
#define SD_GRAPHICS_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/DeferredRenderPass.hpp"
#include "Renderer/SkyboxRenderPass.hpp"
#include "Renderer/PostProcessRenderPass.hpp"
#include "Renderer/SpriteRenderPass.hpp"
#include "Resource/ResourceManager.hpp"
#include "ECS/SceneManager.hpp"
#include "Utility/Timing.hpp"

namespace SD {

class SD_CORE_API GraphicsLayer : public Layer {
   public:
    GraphicsLayer(ResourceManager* resources, SceneManager* scenes,
                  Device* device, int32_t width, int32_t height,
                  MultiSampleLevel msaa);
    void OnImGui() override;
    void OnRender() override;
    void OnTick(float dt) override;

    void SetRenderSize(int32_t width, int32_t height);
    void SetCamera(Camera* camera);
    void SetDebug(bool debug) { m_debug = debug; }

    Framebuffer* GetFramebuffer() { return m_main_target.get(); }

    void OutputColorBuffer(Framebuffer* framebuffer, int attachment);

    uint32_t ReadEntityId(int x, int y) const;

   private:
    void InitBuffers();

    ResourceManager* m_resources;
    SceneManager* m_scenes;
    Device* m_device;
    int32_t m_width;
    int32_t m_height;
    MultiSampleLevel m_msaa;
    bool m_debug;

    Ref<Framebuffer> m_main_target;

    Ref<Texture> m_color_buffer;
    Framebuffer* m_color_output;
    int32_t m_color_output_attachment;

    Ref<Renderbuffer> m_depth_buffer;

    TextureHandle m_light_icon;

    Camera* m_camera;
    FPSCounter m_fps;
    float m_deferred_time;
    float m_post_rendering_time;
};

}  // namespace SD

#endif
