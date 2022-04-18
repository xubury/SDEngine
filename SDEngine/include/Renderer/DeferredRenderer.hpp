#ifndef SD_DEFERRED_RENDERER_HPP
#define SD_DEFERRED_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/CascadeShadow.hpp"
#include "Graphics/PointShadow.hpp"

namespace SD {

enum class GeometryBufferType {
    Position = 0,
    Normal,
    Albedo,
    Ambient,
    Emissive,
    EntityId,
    GBufferCount
};

struct DeferredRenderSettings {
    int32_t width;
    int32_t height;
    MultiSampleLevel msaa;
    bool ssao_state{true};
    float ssao_radius{2.0f};
    float ssao_bias{0.5};
    int ssao_power{3};
};

DataFormat SD_RENDERER_API GetTextureFormat(GeometryBufferType type);

class SD_RENDERER_API DeferredRenderer {
   public:
    static void Init(DeferredRenderSettings settings);

    static void Render(Scene &scene, const Camera &camera);

    static void ImGui();

    static void SetRenderSize(int32_t width, int32_t height);

   private:
    static void InitShaders();
    static void InitSSAOBuffers();
    static void InitSSAOKernel();
    static void InitLightingBuffers();

    static void RenderGBuffer(const Scene &scene);
    static void BlitGeometryBuffers();

    static void RenderSSAO();

    static void RenderShadowMap(const Scene &scene, CascadeShadow &shadow,
                                const Camera &camera,
                                const Transform &transform);
    static void RenderPointShadowMap(const Scene &scene, PointShadow &shadow,
                                     const Transform &transform);
    static void RenderDeferred(Scene &scene, const Camera &camera);
    static void RenderEmissive();
};

}  // namespace SD

#endif /* SD_LIGHTING_SYSTEM_HPP */
