#ifndef SD_POST_PROCESS_RENDERER_HPP
#define SD_POST_PROCESS_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Framebuffer.hpp"

namespace SD {

struct PostProcessSettings {
    int width;
    int height;

    float bloom_threshold{1.0};
    float bloom_soft_threshold{0.8};

    bool is_bloom{true};
    float exposure{1.0};
    float gamma_correction{1.5};
};

class SD_RENDERER_API PostProcessRenderer {
   public:
    static void Init(const PostProcessSettings &settings);

    static void SetRenderSize(int32_t width, int32_t height);

    static void ImGui();

    static void Render();

   private:
    static void RenderPost();

    static void RenderBloom();
    static void Downsample(Texture &src, Texture &dst);
    static void Upsample(Texture &src, Texture &dst);

    static void InitBuffers();
};

}  // namespace SD
#endif
